//
// Created by bkg2k on 07/12/2019.
//

#include <EmulationStation.h>
#include <utils/Strings.h>
#include <utils/os/system/Thread.h>
#include "ScreenScraperApis.h"
#include <utils/Log.h>

std::string ScreenScraperApis::BuildUrlCommon(ScreenScraperApis::Api api)
{
  // Url
  std::string result("https://www.screenscraper.fr/api2/");
  // Api
  switch(api)
  {
    case Api::UserInfo: result.append("ssuserInfos.php?"); break;
    case Api::GameInfo: result.append("jeuInfos.php?"); break;
  }
  // Format
  result.append("output=json");
  // Dev
  result.append("&devid=").append(XOrTheSpaceSheriff(API_DEV_U, API_DEV_K));
  result.append("&devpassword=").append(XOrTheSpaceSheriff(API_DEV_P, API_DEV_K));
  // Software
  result.append("&softname=").append("Emulationstation-Recalbox-" + Strings::Trim(PROGRAM_VERSION_STRING));
  // Credentials
  result.append("&ssid=").append(mConfiguration.GetLogin());
  result.append("&sspassword=").append(mConfiguration.GetPassword());

  return result;
}

std::string ScreenScraperApis::BuildUrlGameInfo(int system, const Path& path, const std::string& crc32,
                                                const std::string& md5, long long size)
{
  // Build the common part
  std::string result(BuildUrlCommon(Api::GameInfo));

  // Add gameinfo properties
  result.append("&romtype=rom");
  result.append("&systemeid=").append(Strings::ToString(system));
  result.append("&romnom=").append(Strings::URLEncode(path.Filename()));
  result.append("&romtaille=").append(Strings::ToString(size));
  if (!crc32.empty())
    result.append("&crc=").append(crc32);
  if (!md5.empty())
    result.append("&md5=").append(md5);

  return result;
}

ScreenScraperApis::User ScreenScraperApis::GetUserInformation()
{
  User user {};
  std::string output;
  if (mClient.Execute(BuildUrlCommon(Api::UserInfo), output))
  {
    rapidjson::Document json;
    json.Parse(output.c_str());
    if (json.HasMember("response"))
    {
      const rapidjson::Value& response = json["response"];
      if (response.HasMember("ssuser"))
      {
        const rapidjson::Value& ssuser = response["ssuser"];
        if (ssuser.HasMember("maxthreads"))
          if (!Strings::ToInt(ssuser["maxthreads"].GetString(), user.mThreads)) user.mThreads = 1;
        if (ssuser.HasMember("requeststoday"))
          if (!Strings::ToInt(ssuser["requeststoday"].GetString(), user.mRequestDone)) user.mRequestDone = 0;
        if (ssuser.HasMember("maxrequestspermin"))
          if (!Strings::ToInt(ssuser["maxrequestspermin"].GetString(), user.mMaxRatePerMin)) user.mMaxRatePerMin = 0;
        if (ssuser.HasMember("maxrequestsperday"))
          if (!Strings::ToInt(ssuser["maxrequestsperday"].GetString(), user.mMaxRatePerDay)) user.mMaxRatePerDay = 0;
        if (ssuser.HasMember("favregion")) ssuser["favregion"].GetString();
      }
    }
  }
  return user;
}

ScreenScraperApis::Game
ScreenScraperApis::GetGameInformation(int system, const Path& path, const std::string& crc32, const std::string& md5, long long size)
{
  Game game {};
  // 3 retry max
  for(int i = 3; --i >= 0; )
  {
    std::string output;
    std::string url(BuildUrlGameInfo(system, path, crc32, md5, size));
    if (mClient.Execute(url, output))
    {
      switch(mClient.GetLastHttpResponseCode())
      {
        case   0:
        case 429: Thread::Sleep(5000); continue;
        case 426:
        case 423:
        case 403:
        case 401:
        case 400: game.mResult = ScrapeResult::FatalError; break;
        case 404: game.mResult = ScrapeResult::NotFound; break;
        case 431:
        case 430: game.mResult = ScrapeResult::QuotaReached; break;
        case 200: game.mResult = ScrapeResult::Ok; break;
        default:
        {
          LOG(LogError) << "Unexpected HTTP return code " << mClient.GetLastHttpResponseCode();
          game.mResult = ScrapeResult::FatalError;
          break;
        }
      }
      // Error?
      if (game.mResult != ScrapeResult::Ok) break;
      if (output.empty())
      {
        LOG(LogError) << "Empty response, retrying...";
        continue;
      }

      // Deserialize
      DeserializeGameInformation(output, game);
      break;
    }
  }
  return game;
}

void ScreenScraperApis::DeserializeGameInformation(const std::string& jsonstring, ScreenScraperApis::Game& game)
{
  rapidjson::Document json;
  json.Parse(jsonstring.c_str());
  if (json.HasMember("response"))
  {
    const rapidjson::Value& response = json["response"];
    if (response.HasMember("jeu"))
    {
      const rapidjson::Value& jeu = response["jeu"];

      // Deserialize text data
      if (jeu.HasMember("noms"))
        game.mName = ExtractRegionalizedText(jeu["noms"], mConfiguration.GetFavoriteRegion());
      if (jeu.HasMember("synopsis"))
       game.mSynopsis = ExtractLocalizedText(jeu["synopsis"], mConfiguration.GetFavoriteLanguage());
      if (jeu.HasMember("editeur"))
        game.mPublisher = jeu["editeur"]["text"].GetString();
      if (jeu.HasMember("developpeur"))
        game.mDeveloper = jeu["developpeur"]["text"].GetString();
      if (jeu.HasMember("joueurs"))
        game.mPlayers = jeu["joueurs"]["text"].GetString();
      if (jeu.HasMember("dates"))
      {
        std::string dateTime = ExtractRegionalizedText(jeu["dates"], mConfiguration.GetFavoriteRegion());
        if (!DateTime::ParseFromString("%yyyy-%MM-%dd", dateTime, game.mReleaseDate))
          if (!DateTime::ParseFromString("%yyyy-%MM", dateTime, game.mReleaseDate))
            DateTime::ParseFromString("%yyyy", dateTime, game.mReleaseDate);
      }
      if (jeu.HasMember("note"))
      {
        int rating;
        game.mRating = (float) (Strings::ToInt(jeu["note"]["text"].GetString(), rating) ? rating : 0) / 20.0f;
      }

      // Deserialize media url
      if (jeu.HasMember("medias"))
      {
        const rapidjson::Value& medias = jeu["medias"];

        // Image
        const char* type  = "mixrbv1"; // default to screenshot
        const char* type2 = nullptr;   // No default type2
        switch (mConfiguration.GetImageType())
        {
          case IConfiguration::Image::None: break;
          case IConfiguration::Image::ScreenshotIngame: type = "ss"; break;
          case IConfiguration::Image::ScreenshotTitle: type = "sstitle"; type2 = "ss"; break;
          case IConfiguration::Image::Box2d: type = "box-2D"; break;
          case IConfiguration::Image::Box3d: type = "box-3D"; break;
          case IConfiguration::Image::MixV1: type = "mixrbv1"; break;
          case IConfiguration::Image::MixV2: type = "mixrbv2"; break;
          case IConfiguration::Image::Wheel: type = "wheel-hd"; type2 = "wheel"; break;
          case IConfiguration::Image::Marquee: type = "screenmarqueesmall"; break;
        }
        if (mConfiguration.GetImageType() != IConfiguration::Image::None)
        {
          game.MediaSources.mImage = ExtractMedia(medias, type, mConfiguration.GetFavoriteRegion(), game.MediaSources.mImageFormat, game.MediaSources.mImageSize);
          if (game.MediaSources.mImage.empty() && type2 != nullptr)
            game.MediaSources.mImage = ExtractMedia(medias, type2, mConfiguration.GetFavoriteRegion(), game.MediaSources.mImageFormat, game.MediaSources.mImageSize);
        }

        // Thumbnail
        type  = "box-2D"; // default to screenshot
        type2 = nullptr;   // No default type2
        switch (mConfiguration.GetThumbnailType())
        {
          case IConfiguration::Image::None: break;
          case IConfiguration::Image::ScreenshotIngame: type = "ss"; break;
          case IConfiguration::Image::ScreenshotTitle: type = "sstitle"; type2 = "ss"; break;
          case IConfiguration::Image::Box2d: type = "box-2D"; break;
          case IConfiguration::Image::Box3d: type = "box-3D"; break;
          case IConfiguration::Image::MixV1: type = "mixrbv1"; break;
          case IConfiguration::Image::MixV2: type = "mixrbv2"; break;
          case IConfiguration::Image::Wheel: type = "wheel-hd"; type2 = "wheel"; break;
          case IConfiguration::Image::Marquee: type = "screenmarqueesmall"; break;
        }
        if (mConfiguration.GetThumbnailType() != IConfiguration::Image::None)
        {
          game.MediaSources.mThumbnail = ExtractMedia(medias, type, mConfiguration.GetFavoriteRegion(), game.MediaSources.mThumbnailFormat, game.MediaSources.mThumbnailSize);
          if (game.MediaSources.mThumbnail.empty() && type2 != nullptr)
            game.MediaSources.mThumbnail = ExtractMedia(medias, type2, mConfiguration.GetFavoriteRegion(), game.MediaSources.mThumbnailFormat, game.MediaSources.mThumbnailSize);
        }

        // Video
        type = nullptr;
        type2 = nullptr;
        switch (mConfiguration.GetVideo())
        {
          case IConfiguration::Video::None: break;
          case IConfiguration::Video::Raw: type = "video"; break;
          case IConfiguration::Video::Optimized: type = "video-normalized"; type2 = "video"; break;
        }
        if (mConfiguration.GetVideo() != IConfiguration::Video::None)
        {
          game.MediaSources.mVideo = ExtractMedia(medias, type, std::string(), game.MediaSources.mVideoFormat, game.MediaSources.mVideoSize);
          if (game.MediaSources.mVideo.empty() && type2 != nullptr)
            game.MediaSources.mVideo = ExtractMedia(medias, type2, std::string(), game.MediaSources.mVideoFormat, game.MediaSources.mVideoSize);
        }
      }
    }
  }
}

std::string ScreenScraperApis::ExtractRegionalizedText(const rapidjson::Value& array, const std::string& preferedregion)
{
  // Prefered first
  for(auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), preferedregion.c_str()) == 0)
      return object["text"].GetString();

  // Then world?
  for(auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "wor") == 0)
      return object["text"].GetString();

  // Mmmmh... then us?
  for(auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "us") == 0)
      return object["text"].GetString();

  // Mmmmmmmmmmh... then jp?
  for(auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "jp") == 0)
      return object["text"].GetString();

  // Damn!... then ss?
  for(auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "ss") == 0)
      return object["text"].GetString();

  // Oh f***, take the first!
  for(auto& object : array.GetArray())
    return object["text"].GetString();

  // WTFH?!
  LOG(LogError) << "Can't find regionalized text!";
  return std::string();
}

std::string ScreenScraperApis::ExtractLocalizedText(const rapidjson::Value& array, const std::string& preferedlanguage)
{
  // Prefered first
  for(auto& object : array.GetArray())
    if (strcmp(object["langue"].GetString(), preferedlanguage.c_str()) == 0)
      return object["text"].GetString();

  // Mmmmh... then us?
  for(auto& object : array.GetArray())
    if (strcmp(object["langue"].GetString(), "en") == 0)
      return object["text"].GetString();

  // Nope, take the first!
  for(auto& object : array.GetArray())
    return object["text"].GetString();

  // WTFH?!
  LOG(LogError) << "Can't find localized text!";
  return std::string();
}

std::string ScreenScraperApis::ExtractMedia(const rapidjson::Value& medias, const char* type, const std::string& region, std::string& format, long long& size)
{
  const char* parent = "jeu";
  format.clear();
  size = 0;

  if (!region.empty())
  {
    // Prefered first
    for (auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), region.c_str()) == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Then world?
    for (auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "wor") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Mmmmh... then us?
    for (auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "us") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Mmmmmmmmmmh... then jp?
    for (auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "jp") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Damn!... then ss?
    for (auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "ss") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }
  }

  // Oh f***, take the first!
  for(auto& object : medias.GetArray())
    if (strcmp(object["type"].GetString(), type) == 0)
      if (strcmp(object["parent"].GetString(), parent) == 0)
      {
        Strings::ToLong(object["size"].GetString(), size);
        format = object["format"].GetString();
        return object["url"].GetString();
      }

  // WTFH?!
  LOG(LogError) << "Can't find media url!";
  return std::string();
}

ScrapeResult ScreenScraperApis::GetMedia(const std::string& mediaurl, const Path& to, long long& size)
{
  ScrapeResult result = ScrapeResult::FatalError;
  size = 0;
  for(int i = 3; --i >= 0; )
  {
    if (mClient.Execute(mediaurl, to))
      switch (mClient.GetLastHttpResponseCode())
      {
        case 0:
        case 429: Thread::Sleep(5000); continue;
        case 426:
        case 423:
        case 403:
        case 401:
        case 400: result = ScrapeResult::FatalError; break;
        case 404: result = ScrapeResult::NotFound; break;
        case 431:
        case 430: result = ScrapeResult::QuotaReached; break;
        case 200: result = ScrapeResult::Ok; size = mClient.GetLastContentSize(); break;
        default:
        {
          LOG(LogError) << "Unexpected HTTP return code " << mClient.GetLastHttpResponseCode();
          result = ScrapeResult::FatalError;
          break;
        }
      }
    break;
  }
  return result;
}


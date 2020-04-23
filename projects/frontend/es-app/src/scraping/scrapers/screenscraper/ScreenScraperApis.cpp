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
    LOG(LogDebug) << "GetUserInfo: HTTP Result code = " << mClient.GetLastHttpResponseCode() << "\n" << output;

    rapidjson::Document json;
    json.Parse(output.c_str());
    if (!json.HasParseError())
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
      if (game.mResult != ScrapeResult::Ok)
      {
        LOG(LogError) << "Media URL: " << url << " - HTTP Result code = " << mClient.GetLastHttpResponseCode();
        break;
      }
      if (output.empty())
      {
        LOG(LogError) << "Empty response, retrying...";
        continue;
      }

      // Deserialize
      DeserializeGameInformation(output, game);
      break;
    }
    else LOG(LogError) << "Error executing HTTP request onto " << url;
  }
  return game;
}

void ScreenScraperApis::DeserializeGameInformation(const std::string& jsonstring, ScreenScraperApis::Game& game)
{
  rapidjson::Document json;
  json.Parse(jsonstring.c_str());
  if (!json.HasParseError())
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
        {
          game.mSynopsis = ExtractLocalizedText(jeu["synopsis"], mConfiguration.GetFavoriteLanguage());
          DecodeString(game.mSynopsis);
        }
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
        if (jeu.HasMember("genres"))
        {
          game.mGenre = ExtractLocalizedGenre(jeu["genres"], mConfiguration.GetFavoriteLanguage());
          game.mGenreId = ExtractNormalizedGenre(jeu["genres"]);
          game.mAdult = ExtractAdultState(jeu["genres"]);
        }
        if (jeu.HasMember("note"))
        {
          int rating = 0;
          game.mRating = (float) (Strings::ToInt(jeu["note"]["text"].GetString(), rating) ? rating : 0) / 20.0f;
        }
        // Rom infos
        if (jeu.HasMember("rom"))
        {
          const rapidjson::Value& rom = jeu["rom"];
          if (rom.HasMember("romregions"))
            game.mRegion = rom["romregions"].GetString();
          if (rom.HasMember("romcrc"))
            game.mCrc = rom["romcrc"].GetString();
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

          // Marquee
          if (mConfiguration.GetWantMarquee())
          {
            game.MediaSources.mMarquee = ExtractMedia(medias, "screenmarquee", mConfiguration.GetFavoriteRegion(), game.MediaSources.mMarqueeFormat, game.MediaSources.mMarqueeSize);
            if (game.MediaSources.mMarquee.empty() && type2 != nullptr)
              game.MediaSources.mMarquee = ExtractMedia(medias, "screenmarqueesmall", mConfiguration.GetFavoriteRegion(), game.MediaSources.mMarqueeFormat, game.MediaSources.mMarqueeSize);
          }

          // Wheel
          if (mConfiguration.GetWantWheel())
          {
            game.MediaSources.mWheel = ExtractMedia(medias, "wheel-hs", mConfiguration.GetFavoriteRegion(), game.MediaSources.mWheelFormat, game.MediaSources.mWheelSize);
            if (game.MediaSources.mWheel.empty() && type2 != nullptr)
              game.MediaSources.mWheel = ExtractMedia(medias, "wheel", mConfiguration.GetFavoriteRegion(), game.MediaSources.mWheelFormat, game.MediaSources.mWheelSize);
          }

          // Manual
          if (mConfiguration.GetWantManual())
            game.MediaSources.mManual = ExtractMedia(medias, "manuel", mConfiguration.GetFavoriteRegion(), game.MediaSources.mManualFormat, game.MediaSources.mManualSize);

          // Maps
          if (mConfiguration.GetWantMaps())
            game.MediaSources.mMaps = ExtractMedia(medias, "maps", std::string(), game.MediaSources.mMapsFormat, game.MediaSources.mMapsSize);
        }
      }
    }
}

std::string ScreenScraperApis::ExtractRegionalizedText(const rapidjson::Value& array, const std::string& preferedregion)
{
  // Prefered first
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), preferedregion.c_str()) == 0)
      return object["text"].GetString();

  // Then world?
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "wor") == 0)
      return object["text"].GetString();

  // Mmmmh... then us?
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "us") == 0)
      return object["text"].GetString();

  // Mmmmmmmmmmh... then jp?
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "jp") == 0)
      return object["text"].GetString();

  // Damn!... then ss?
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), "ss") == 0)
      return object["text"].GetString();

  // Oh f***, take the first!
  for(const auto& object : array.GetArray())
    return object["text"].GetString();

  // WTFH?!
  LOG(LogError) << "Can't find regionalized text!";
  return std::string();
}

std::string ScreenScraperApis::ExtractLocalizedText(const rapidjson::Value& array, const std::string& preferedlanguage)
{
  // Prefered first
  for(const auto& object : array.GetArray())
    if (strcmp(object["langue"].GetString(), preferedlanguage.c_str()) == 0)
      return object["text"].GetString();

  // Mmmmh... then us?
  for(const auto& object : array.GetArray())
    if (strcmp(object["langue"].GetString(), "en") == 0)
      return object["text"].GetString();

  // Nope, take the first!
  for(const auto& object : array.GetArray())
    return object["text"].GetString();

  // WTFH?!
  LOG(LogError) << "Can't find localized text!";
  return std::string();
}


std::string ScreenScraperApis::ExtractLocalizedGenre(const rapidjson::Value& array, const std::string& preferedlanguage)
{
  std::string result;

  // Prefered first
  for(const auto& genre : array.GetArray())
    for(const auto& nom : genre["noms"].GetArray())
      if (strcmp(nom["langue"].GetString(), preferedlanguage.c_str()) == 0)
      {
        if (!result.empty()) result.append(1, ',');
        result.append(nom["text"].GetString());
      }
  if (!result.empty()) return result;

  // Mmmmh... then us?
  for(const auto& genre : array.GetArray())
    for(const auto& nom : genre["noms"].GetArray())
      if (strcmp(nom["langue"].GetString(), "en") == 0)
      {
        if (!result.empty()) result.append(1, ',');
        result.append(nom["text"].GetString());
      }
  return result;
}

GameGenres ScreenScraperApis::ExtractNormalizedGenre(const rapidjson::Value& array)
{
  static HashMap<int, GameGenres> sScreenScraperSubGenresToGameGenres
  ({
    {  2909, GameGenres::  ActionPlatformer             },
    {     7, GameGenres::  ActionPlatformer             },
    {  2915, GameGenres::  ActionPlatformer             },
    {  2897, GameGenres::  ActionPlatformer             },
    {  2937, GameGenres::  ActionPlatformShooter        },
    {  3026, GameGenres::  ActionPlatformShooter        },
    {  2887, GameGenres::  ActionPlatformShooter        },
    {  2896, GameGenres::  ActionPlatformShooter        },
    {  3024, GameGenres::  ActionFirstPersonShooter     },
    {  2988, GameGenres::  ActionFirstPersonShooter     },
    {    79, GameGenres::  ActionShootEmUp              },
    {  2955, GameGenres::  ActionShootEmUp              },
    {  2870, GameGenres::  ActionShootEmUp              },
    {  2851, GameGenres::  ActionShootEmUp              },
    {  2876, GameGenres::  ActionShootEmUp              },
    {  2900, GameGenres::  ActionShootEmUp              },
    {  2889, GameGenres::  ActionShootEmUp              },
    {  2945, GameGenres::  ActionShootWithGun           },
    {    32, GameGenres::  ActionShootWithGun           },
    {    14, GameGenres::  ActionFighting               },
    {  2874, GameGenres::  ActionFighting               },
    {  2914, GameGenres::  ActionFighting               },
    {  2920, GameGenres::  ActionFighting               },
    {  2885, GameGenres::  ActionFighting               },
    {  2957, GameGenres::  ActionFighting               },
    {  2922, GameGenres::  ActionFighting               },
    {     1, GameGenres::  ActionBeatEmUp               },
    {   425, GameGenres::  ActionRythm                  },
    {  2925, GameGenres::  ActionRythm                  },
    {  3237, GameGenres::  AdventureGraphics            },
    { 20672, GameGenres::  AdventureGraphics            },
    { 20678, GameGenres::  AdventureRealTime3D          },
    { 20676, GameGenres::  AdventureInteractiveMovie    },
    { 20674, GameGenres::  AdventureVisualNovels        },
    { 20680, GameGenres::  AdventureSurvivalHorror      },
    { 20670, GameGenres::  AdventureText                },
    {  2648, GameGenres::  SimulationFishAndHunt        },
    {  3192, GameGenres::  SimulationFishAndHunt        },
    {  2895, GameGenres::  SimulationFishAndHunt        },
    {  2893, GameGenres::  SimulationVehicle            },
    {  2911, GameGenres::  SimulationVehicle            },
    {  2953, GameGenres::  SimulationVehicle            },
    {  2940, GameGenres::  SimulationVehicle            },
    {  2910, GameGenres::  SimulationVehicle            },
    {  2934, GameGenres::  SimulationVehicle            },
    {  2938, GameGenres::  SimulationVehicle            },
    {  2921, GameGenres::  SimulationVehicle            },
    {    28, GameGenres::  SportRacing                  },
    {  2924, GameGenres::  SportRacing                  },
    {  2973, GameGenres::  SportRacing                  },
    {  2871, GameGenres::  SportRacing                  },
    {  2884, GameGenres::  SportRacing                  },
    {  2943, GameGenres::  SportRacing                  },
    {  2853, GameGenres::  SportSimulation              },
    {  2852, GameGenres::  SportSimulation              },
    {  3028, GameGenres::  SportSimulation              },
    {  2901, GameGenres::  SportSimulation              },
    {  2877, GameGenres::  SportSimulation              },
    {  2970, GameGenres::  SportSimulation              },
    {  2906, GameGenres::  SportSimulation              },
    {  2847, GameGenres::  SportSimulation              },
    {  2846, GameGenres::  SportSimulation              },
    {  2913, GameGenres::  SportSimulation              },
    {  2960, GameGenres::  SportSimulation              },
    {  2933, GameGenres::  SportSimulation              },
    {  2971, GameGenres::  SportSimulation              },
    {  2878, GameGenres::  SportSimulation              },
    {  2968, GameGenres::  SportSimulation              },
    {  2966, GameGenres::  SportSimulation              },
    {  2948, GameGenres::  SportSimulation              },
    {  2875, GameGenres::  SportSimulation              },
    {  2902, GameGenres::  SportSimulation              },
    {  2867, GameGenres::  SportSimulation              },
    {  2883, GameGenres::  SportSimulation              },
    {  2650, GameGenres::  SportSimulation              },
    {  2918, GameGenres::  SportSimulation              },
    {  2929, GameGenres::  SportFight                   },
    {  2919, GameGenres::  SportFight                   },
    {  3034, GameGenres::  SportFight                   },
    {  1861, GameGenres::  SportFight                   },
    {  2947, GameGenres::  SportFight                   },
  });

  static HashMap<int, GameGenres> sScreenScraperGenresToGameGenres
  ({
    { 2855, GameGenres::None           },
    { 2904, GameGenres::None           },
    { 2879, GameGenres::None           },
    {   10, GameGenres::Action         },
    { 2903, GameGenres::Action         },
    { 2928, GameGenres::Action         },
    { 2881, GameGenres::Action         },
    { 2646, GameGenres::Action         },
    { 3566, GameGenres::Action         },
    { 2917, GameGenres::Action         },
    {   13, GameGenres::Adventure      },
    {    8, GameGenres::RPG            },
    { 2890, GameGenres::Simulation     },
    {   40, GameGenres::Simulation     },
    {   27, GameGenres::Strategy       },
    {  685, GameGenres::Sports         },
    {   31, GameGenres::Pinball        },
    {   33, GameGenres::Board          },
    { 2647, GameGenres::Board          },
    { 2958, GameGenres::Board          },
    { 2882, GameGenres::Board          },
    { 2869, GameGenres::Board          },
    { 2949, GameGenres::Board          },
    { 2956, GameGenres::Board          },
    { 2961, GameGenres::Board          },
    {  171, GameGenres::Casual         },
    {   42, GameGenres::DigitalCard    },
    { 3511, GameGenres::PuzzleAndLogic },
    {   26, GameGenres::PuzzleAndLogic },
    { 2864, GameGenres::PuzzleAndLogic },
    { 2891, GameGenres::PuzzleAndLogic },
    { 2923, GameGenres::PuzzleAndLogic },
    { 2912, GameGenres::PuzzleAndLogic },
    { 2649, GameGenres::Trivia         },
    { 2954, GameGenres::Trivia         },
    { 2931, GameGenres::Trivia         },
    { 2951, GameGenres::Trivia         },
    { 2962, GameGenres::Trivia         },
    { 2969, GameGenres::Trivia         },
    { 2952, GameGenres::Trivia         },
    { 2894, GameGenres::Trivia         },
    { 2964, GameGenres::Trivia         },
    { 2967, GameGenres::Trivia         },
    {  320, GameGenres::Casino         },
    { 2872, GameGenres::Casino         },
    { 2950, GameGenres::Casino         },
    { 2932, GameGenres::Casino         },
    { 2860, GameGenres::Casino         },
    { 2944, GameGenres::Casino         },
    {   34, GameGenres::Compilation    },
    { 2974, GameGenres::DemoScene      },
    {   30, GameGenres::Educative      },
  });
  
  // Lookup Sub-genre first
  int id = 0;
  for(const auto& object : array.GetArray())
    if (Strings::ToInt(object["id"].GetString(), id))
    {
      GameGenres* found = sScreenScraperSubGenresToGameGenres.try_get(id);
      if (found != nullptr)
        return *found;
    }

  // Lookup genre except "Action"
  for(const auto& object : array.GetArray())
    if (Strings::ToInt(object["id"].GetString(), id))
      if (id != 10 && id != 413) // Action / Adult
      {
        GameGenres* found = sScreenScraperGenresToGameGenres.try_get(id);
        if (found != nullptr)
          return *found;
      }

  // Lookup what's available
  for(const auto& object : array.GetArray())
    if (Strings::ToInt(object["id"].GetString(), id))
      if (id != 413) // Adult
      {
        GameGenres* found = sScreenScraperGenresToGameGenres.try_get(id);
        if (found != nullptr)
          return *found;
      }

  return GameGenres::None;
}

bool ScreenScraperApis::ExtractAdultState(const rapidjson::Value& array)
{
  // Lookup Sub-genre first
  int id = 0;
  for(const auto& object : array.GetArray())
    if (Strings::ToInt(object["id"].GetString(), id))
      if (id == 413)
        return true;
  return false;
}

std::string ScreenScraperApis::ExtractMedia(const rapidjson::Value& medias, const char* type, const std::string& region, std::string& format, long long& size)
{
  const char* parent = "jeu";
  format.clear();
  size = 0;

  if (!region.empty())
  {
    // Prefered first
    for (const auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), region.c_str()) == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Then world?
    for (const auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "wor") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Mmmmh... then us?
    for (const auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "us") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Mmmmmmmmmmh... then jp?
    for (const auto& object : medias.GetArray())
      if (strcmp(object["type"].GetString(), type) == 0)
        if (strcmp(object["parent"].GetString(), parent) == 0)
          if (strcmp(object["region"].GetString(), "jp") == 0)
          {
            Strings::ToLong(object["size"].GetString(), size);
            format = object["format"].GetString();
            return object["url"].GetString();
          }

    // Damn!... then ss?
    for (const auto& object : medias.GetArray())
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
  for(const auto& object : medias.GetArray())
    if (strcmp(object["type"].GetString(), type) == 0)
      if (strcmp(object["parent"].GetString(), parent) == 0)
      {
        Strings::ToLong(object["size"].GetString(), size);
        format = object["format"].GetString();
        return object["url"].GetString();
      }

  // WTFH?!
  LOG(LogDebug) << "Can't find media url matching: " << type << " / " << region;
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

  if (result != ScrapeResult::Ok)
    LOG(LogError) << "Media URL: " << mediaurl << " - HTTP Result code = " << mClient.GetLastHttpResponseCode();

  // Delete wrong files
  if (to.Size() <= 256)
  {
    to.Delete();
    size = 0;
  }

  return result;
}

void ScreenScraperApis::DecodeString(std::string& raw)
{
  // XML
  raw = Strings::Replace(raw, "&quot;", "\"");
  raw = Strings::Replace(raw, "&amp;", "&");
  raw = Strings::Replace(raw, "&apos;", "'");
  raw = Strings::Replace(raw, "&lt;", "<");
  raw = Strings::Replace(raw, "&gt;", ">");
  raw = Strings::Replace(raw, "&nbsp;", "\u0160");
}

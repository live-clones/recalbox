//
// Created by bkg2k on 07/12/2019.
//

#include <utils/Strings.h>
#include <utils/os/system/Thread.h>
#include "ScreenScraperApis.h"
#include <utils/Log.h>
#include <games/classifications/Regions.h>
#include <scraping/ScraperFactory.h>
#include <scraping/ScraperSeamless.h>

ScreenScraperUser ScreenScraperApis::GetUserInformation()
{
  ScreenScraperUser user;

  InitializeClient();
  std::string output;
  if (mClient.Execute(mEndPointProvider.GetUserInfoUrl(mConfiguration.GetLogin(), mConfiguration.GetPassword()), output))
  {
    { LOG(LogDebug) << "[ScreenScraperApis] GetUserInfo: HTTP Result code = " << mClient.GetLastHttpResponseCode() << "\n" << output; }

    rapidjson::Document json;
    json.Parse(output.c_str());
    if (!json.HasParseError() && json.HasMember("response"))
    {
      const rapidjson::Value& response = json["response"];
      if (response.HasMember("ssuser"))
      {
        const rapidjson::Value& ssuser = response["ssuser"];
        user.mThreads = 1;
        user.mRequestDone = 0;
        user.mMaxRatePerMin = 0;
        user.mMaxRatePerDay = 0;
        if (ssuser.HasMember("maxthreads")) Strings::ToInt(ssuser["maxthreads"].GetString(), user.mThreads);
        if (ssuser.HasMember("requeststoday")) Strings::ToInt(ssuser["requeststoday"].GetString(), user.mRequestDone);
        if (ssuser.HasMember("maxrequestspermin")) Strings::ToInt(ssuser["maxrequestspermin"].GetString(), user.mMaxRatePerMin);
        if (ssuser.HasMember("maxrequestsperday")) Strings::ToInt(ssuser["maxrequestsperday"].GetString(), user.mMaxRatePerDay);
        if (ssuser.HasMember("favregion")) ssuser["favregion"].GetString();
      }
    }
  }
  return user;
}

ScreenScraperApis::Game
ScreenScraperApis::GetGameInformation(const FileData& file, const std::string& crc32, const std::string& md5, long long size)
{
  Game game {};
  // 3 retry max
  InitializeClient();
  for(int i = 3; --i >= 0; )
  {
    std::string url;
    if (mEndPointProvider.RequireSeparateRequests())
      url = md5.empty() ?
            mEndPointProvider.GetGameInfoUrlByName(mConfiguration.GetLogin(), mConfiguration.GetPassword(), file, md5, size) :
            mEndPointProvider.GetGameInfoUrlByMD5(mConfiguration.GetLogin(), mConfiguration.GetPassword(), file, md5, size);
    else
      url = mEndPointProvider.GetGameInfoUrl(mConfiguration.GetLogin(), mConfiguration.GetPassword(), file, crc32, md5, size);
    std::string output;
    if (mClient.Execute(url, output))
    {
      switch(mClient.GetLastHttpResponseCode())
      {
        case   0:
        case 429:
        case 499:
        case 504:
        case 500: LOG(LogError) << "[ScreenScraperApis] Server error " << mClient.GetLastHttpResponseCode() << ", retrying..."; Thread::Sleep(5000); continue;
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
          { LOG(LogError) << "[ScreenScraperApis] Unexpected HTTP return code " << mClient.GetLastHttpResponseCode(); }
          game.mResult = ScrapeResult::FatalError;
          break;
        }
      }
      // Error?
      if (game.mResult != ScrapeResult::Ok) { LOG(LogError) << "[ScreenScraperApis] URL: " << url << " - HTTP Result code = " << mClient.GetLastHttpResponseCode(); }
    }
    else { LOG(LogError) << "[ScreenScraperApis] Error executing HTTP request #1 onto " << url; }

    // Deserialize
    if (!output.empty()) DeserializeGameInformationOuter(output, game, file.FilePath());
    break;
  }
  return game;
}

void ScreenScraperApis::DeserializeGameInformationInner(const rapidjson::Value& json, ScreenScraperApis::Game& game, const Path& path)
{
  Regions::GameRegions romRegion = Regions::GameRegions::Unknown;
  // Rom infos
  if (json.HasMember("rom"))
  {
    const rapidjson::Value& rom = json["rom"];
    if (rom.HasMember("romregions"))
    {
      romRegion = Regions::DeserializeRegion(rom["romregions"].GetString());
    }
    if (rom.HasMember("romcrc"))
      game.mCrc = rom["romcrc"].GetString();
  }
  std::string requiredRegion = ScreenScraperApis::GetRequiredRegion(romRegion, path,
                                                                    mConfiguration.GetFavoriteRegion());
  game.mRegion = requiredRegion;
  const std::string language = LanguagesTools::SerializeLanguage(mConfiguration.GetFavoriteLanguage());

  // Deserialize text data
  if (json.HasMember("noms"))
  {
    game.mName = ExtractRegionalizedText(json["noms"], requiredRegion);
    game.mScreenScraperName = CleanGameName(ExtractRegionalizedText(json["noms"], "ss"));
  }
  if (json.HasMember("synopsis"))
  {
    game.mSynopsis = ExtractLocalizedText(json["synopsis"], language);
    DecodeString(game.mSynopsis);
  }
  if (json.HasMember("editeur"))
    game.mPublisher = json["editeur"]["text"].GetString();
  if (json.HasMember("developpeur"))
    game.mDeveloper = json["developpeur"]["text"].GetString();
  if (json.HasMember("joueurs"))
    game.mPlayers = json["joueurs"]["text"].GetString();
  if (json.HasMember("dates"))
  {
    std::string dateTime = ExtractRegionalizedText(json["dates"], requiredRegion);
    if (!DateTime::ParseFromString("%yyyy-%MM-%dd", dateTime, game.mReleaseDate))
      if (!DateTime::ParseFromString("%yyyy-%MM", dateTime, game.mReleaseDate))
        DateTime::ParseFromString("%yyyy", dateTime, game.mReleaseDate);
  }
  if (json.HasMember("genres"))
  {
    game.mGenre = ExtractLocalizedGenre(json["genres"], language);
    game.mGenreId = ExtractNormalizedGenre(json["genres"]);
    game.mAdult = ExtractAdultState(json["genres"]);
  }
  if (json.HasMember("sp2kcfg"))
  {
    game.mP2k = json["sp2kcfg"].GetString();
  }
  if (json.HasMember("note"))
  {
    int rating = 0;
    game.mRating = (float) (Strings::ToInt(json["note"]["text"].GetString(), rating) ? rating : 0) / 20.0f;
  }

  // Deserialize media url
  if (json.HasMember("medias"))
  {
    const rapidjson::Value& medias = json["medias"];

    // Image
    const char* type  = "mixrbv1"; // default to screenshot
    const char* type2 = nullptr;   // No default type2
    switch (mConfiguration.GetImageType())
    {
      case ScreenScraperEnums::ScreenScraperImageType::None: break;
      case ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame: type = "ss"; break;
      case ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle: type = "sstitle"; type2 = "ss"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Box2d: type = "box-2D"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Box3d: type = "box-3D"; break;
      case ScreenScraperEnums::ScreenScraperImageType::MixV2: type = "mixrbv2"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Wheel: type = "wheel-hd"; type2 = "wheel"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Marquee: type = "screenmarqueesmall"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Unknown:
      case ScreenScraperEnums::ScreenScraperImageType::MixV1: type = "mixrbv1"; break;
    }
    if (mConfiguration.GetImageType() != ScreenScraperEnums::ScreenScraperImageType::None)
    {
      game.MediaSources.mImage.mUrl = ExtractMedia(medias, type, requiredRegion, game.MediaSources.mImage.mFormat, game.MediaSources.mImage.mSize, game.MediaSources.mImage.mMd5);
      if (game.MediaSources.mImage.mUrl.empty() && type2 != nullptr)
        game.MediaSources.mImage.mUrl = ExtractMedia(medias, type2, requiredRegion, game.MediaSources.mImage.mFormat, game.MediaSources.mImage.mSize, game.MediaSources.mImage.mMd5);
    }

    // Thumbnail
    type  = "box-2D"; // default to screenshot
    type2 = nullptr;   // No default type2
    switch (mConfiguration.GetThumbnailType())
    {
      case ScreenScraperEnums::ScreenScraperImageType::None: break;
      case ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame: type = "ss"; break;
      case ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle: type = "sstitle"; type2 = "ss"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Box2d: type = "box-2D"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Box3d: type = "box-3D"; break;
      case ScreenScraperEnums::ScreenScraperImageType::MixV1: type = "mixrbv1"; break;
      case ScreenScraperEnums::ScreenScraperImageType::MixV2: type = "mixrbv2"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Wheel: type = "wheel-hd"; type2 = "wheel"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Marquee: type = "screenmarqueesmall"; break;
      case ScreenScraperEnums::ScreenScraperImageType::Unknown: type = "none"; break;
    }
    if (mConfiguration.GetThumbnailType() != ScreenScraperEnums::ScreenScraperImageType::None)
    {
      game.MediaSources.mThumbnail.mUrl = ExtractMedia(medias, type, requiredRegion, game.MediaSources.mThumbnail.mFormat, game.MediaSources.mThumbnail.mSize, game.MediaSources.mThumbnail.mMd5);
      if (game.MediaSources.mThumbnail.mUrl.empty() && type2 != nullptr)
        game.MediaSources.mThumbnail.mUrl = ExtractMedia(medias, type2, requiredRegion, game.MediaSources.mThumbnail.mFormat, game.MediaSources.mThumbnail.mSize, game.MediaSources.mThumbnail.mMd5);
    }

    // Video
    type = nullptr;
    type2 = nullptr;
    switch (mConfiguration.GetVideo())
    {
      case ScreenScraperEnums::ScreenScraperVideoType::None: break;
      case ScreenScraperEnums::ScreenScraperVideoType::Raw: type = "video"; break;
      case ScreenScraperEnums::ScreenScraperVideoType::Optimized: type = "video-normalized"; type2 = "video"; break;
    }
    if (mConfiguration.GetVideo() != ScreenScraperEnums::ScreenScraperVideoType::None)
    {
      game.MediaSources.mVideo.mUrl = ExtractMedia(medias, type, std::string(), game.MediaSources.mVideo.mFormat, game.MediaSources.mVideo.mSize, game.MediaSources.mVideo.mMd5);
      if (game.MediaSources.mVideo.mUrl.empty() && type2 != nullptr)
        game.MediaSources.mVideo.mUrl = ExtractMedia(medias, type2, std::string(), game.MediaSources.mVideo.mFormat, game.MediaSources.mVideo.mSize, game.MediaSources.mVideo.mMd5);
    }

    // Marquee
    if (mConfiguration.GetWantMarquee())
    {
      game.MediaSources.mMarquee.mUrl = ExtractMedia(medias, "screenmarquee", requiredRegion, game.MediaSources.mMarquee.mFormat, game.MediaSources.mMarquee.mSize, game.MediaSources.mMarquee.mMd5);
      if (game.MediaSources.mMarquee.mUrl.empty() && type2 != nullptr)
        game.MediaSources.mMarquee.mUrl = ExtractMedia(medias, "screenmarqueesmall", requiredRegion, game.MediaSources.mMarquee.mFormat, game.MediaSources.mMarquee.mSize, game.MediaSources.mMarquee.mMd5);
    }

    // Wheel
    if (mConfiguration.GetWantWheel())
    {
      game.MediaSources.mWheel.mUrl = ExtractMedia(medias, "wheel-hs", requiredRegion, game.MediaSources.mWheel.mFormat, game.MediaSources.mWheel.mSize, game.MediaSources.mWheel.mMd5);
      if (game.MediaSources.mWheel.mUrl.empty() && type2 != nullptr)
        game.MediaSources.mWheel.mUrl = ExtractMedia(medias, "wheel", requiredRegion, game.MediaSources.mWheel.mFormat, game.MediaSources.mWheel.mSize, game.MediaSources.mWheel.mMd5);
    }

    // Manual
    if (mConfiguration.GetWantManual())
      game.MediaSources.mManual.mUrl = ExtractMedia(medias, "manuel", requiredRegion, game.MediaSources.mManual.mFormat, game.MediaSources.mManual.mSize, game.MediaSources.mManual.mMd5);

    // Maps
    if (mConfiguration.GetWantMaps())
      game.MediaSources.mMaps.mUrl = ExtractMedia(medias, "maps", std::string(), game.MediaSources.mMaps.mFormat, game.MediaSources.mMaps.mSize, game.MediaSources.mMaps.mMd5);
  }
}

void ScreenScraperApis::DeserializeGameInformationOuter(const std::string& jsonstring, ScreenScraperApis::Game& game, const Path& path)
{
  rapidjson::Document json;
  json.Parse(jsonstring.c_str());
  if (!json.HasParseError())
  {
    // ScreenScraper response
    if (json.HasMember("response"))
    {
      const rapidjson::Value& response = json["response"];
      if (response.HasMember("jeu"))
      {
        const rapidjson::Value& gameJson = response["jeu"];
        DeserializeGameInformationInner(gameJson, game, path);
      }
    }
    // Recalbox
    if (json.HasMember("data"))
    {
      const rapidjson::Value& gameJson = json["data"];
      DeserializeGameInformationInner(gameJson, game, path);
    }
  }
}

std::string ScreenScraperApis::GetRequiredRegion(Regions::GameRegions romRegion, const Path& path, Regions::GameRegions favoriteRegion)
{
  if(RecalboxConf::Instance().GetScreenScraperRegionPriority() == ScreenScraperEnums::ScreenScraperRegionPriority::DetectedRegion)
  {
    Regions::GameRegions regionFromFile = Regions::ExtractRegionsFromFileName(path);
    if(regionFromFile != Regions::GameRegions::Unknown)
      return  Regions::SerializeRegion(regionFromFile);

    if(romRegion != Regions::GameRegions::Unknown)
    return Regions::SerializeRegion(romRegion);
  }
  return Regions::SerializeRegion(favoriteRegion);
}

std::string ScreenScraperApis::ExtractRegionalizedText(const rapidjson::Value& array, const std::string& requiredRegion)
{
  // required first
  for(const auto& object : array.GetArray())
    if (strcmp(object["region"].GetString(), requiredRegion.c_str()) == 0)
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
  { LOG(LogError) << "[ScreenScraperApis] Can't find regionalized text!"; }
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
  { LOG(LogError) << "[ScreenScraperApis] Can't find localized text!"; }
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

std::string ScreenScraperApis::ExtractMedia(const rapidjson::Value& medias, const char* type, const std::string& region, std::string& format, long long& size, std::string& md5)
{
  const char* parent = "jeu";
  format.clear();
  size = 0;
  md5.clear();

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
            md5 = object["md5"].GetString();
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
            md5 = object["md5"].GetString();
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
            md5 = object["md5"].GetString();
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
            md5 = object["md5"].GetString();
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
            md5 = object["md5"].GetString();
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
        md5 = object["md5"].GetString();
        return object["url"].GetString();
      }

  // WTFH?!
  { LOG(LogDebug) << "[ScreenScraperApis] Can't find media url matching: " << type << " / " << region; }
  return std::string();
}

ScrapeResult ScreenScraperApis::GetMedia(const FileData& game, const std::string& mediaurl, const Path& to, long long& size)
{
  ScrapeResult result = ScrapeResult::FatalError;
  size = 0;
  std::string url = Strings::StartsWith(mediaurl,LEGACY_STRING("http")) ? mediaurl : mEndPointProvider.GetUrlBase() + mediaurl;
  InitializeClient();
  mEndPointProvider.AddQueryParametersToMediaRequest(&game, size, url);
  for(int i = 3; --i >= 0; )
  {
    if (mClient.Execute(url, to))
      switch (mClient.GetLastHttpResponseCode())
      {
        case 0:
        case 429:
        case 499:
        case 504:
        case 500: LOG(LogError) << "[ScreenScraperApis] Server error " << mClient.GetLastHttpResponseCode() << ", retrying..."; Thread::Sleep(5000); continue;
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
          { LOG(LogError) << "[ScreenScraperApis] Unexpected HTTP return code " << mClient.GetLastHttpResponseCode(); }
          result = ScrapeResult::FatalError;
          break;
        }
      }
    break;
  }

  if (result != ScrapeResult::Ok)
  { LOG(LogError) << "[ScreenScraperApis] Media URL: " << mediaurl << " - HTTP Result code = " << mClient.GetLastHttpResponseCode(); }

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

std::string ScreenScraperApis::CleanGameName(const std::string& gameName)
{
  std::string result;
  const char* p = gameName.c_str();

  for (int i = (int)gameName.length(); --i >= 0;)
  {
    unsigned char C = (unsigned char)*p++;
    if (((C >= 'a') && (C <= 'z')) ||
        ((C >= 'A') && (C <= 'Z')) ||
        ((C >= '0') && (C <= '9')) ||
        (C == '(') ||
        (C == ')') ||
        (C == '[') ||
        (C == ']') ||
        (C == ' ') ||
        (C == '.') ||
        (C == '_') ||
        (C == '-')) result.append(1, (char)C);
  }

  return result;
}
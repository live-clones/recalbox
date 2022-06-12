//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "RecalboxEndPoints.h"
#include "scraping/ScraperSeamless.h"
#include "Upgrade.h"
#include "hardware/Board.h"
#include "utils/Strings.h"
#include "systems/SystemData.h"

std::string RecalboxEndPoints::GetUserInfoUrl(const std::string& login, const std::string& password)
{
  (void)login;
  (void)password;

  std::string url = GetUrlBase();
  url.append("/api/authenticated")
     .append(BuildQueryString(nullptr, 0));

  return url;
}

std::string
RecalboxEndPoints::GetGameInfoUrlByMD5(const std::string& login, const std::string& password, const FileData& game,
                                       const std::string& md5, long long int size)
{
  (void)login;
  (void)password;
  (void)size;
  (void)game;

  std::string url = GetUrlBase();
  url.append("/api/game/bymd5/")
     .append(md5)
     .append(BuildQueryString(&game, size));

  return url;
}

std::string
RecalboxEndPoints::GetGameInfoUrlByName(const std::string& login, const std::string& password, const FileData& game,
                                        const std::string& md5, long long int size)
{
  (void)login;
  (void)password;

  std::string url = GetUrlBase();

  url.append("/api/game/bysystem/")
     .append(Strings::ToString(game.System().Descriptor().ScreenScaperID()))
     .append("/andname/")
     .append(Strings::URLEncode(game.FilePath().Filename()))
     .append("/andsize/")
     .append(Strings::ToString(size))
     .append(BuildQueryString(&game, size));
  if (!md5.empty())
    url.append("&md5=")
       .append(md5);

  return url;
}

std::string RecalboxEndPoints::BuildQueryString(const FileData* game, long long size)
{
  std::string result("?board=");
  result.append(Strings::URLEncode(mBoard))
        .append(LEGACY_STRING("&uuid="))
        .append(Strings::URLEncode(mUUID))
        .append(LEGACY_STRING("&version="))
        .append(Strings::URLEncode(mVersion));
  if (game != nullptr)
    result.append(LEGACY_STRING("&system="))
          .append(Strings::ToString(game->System().Descriptor().ScreenScaperID()))
          .append(LEGACY_STRING("&systemname="))
          .append(Strings::URLEncode(game->System().Name()))
          .append(LEGACY_STRING("&gamename="))
          .append(Strings::URLEncode(game->FilePath().Filename()))
          .append(LEGACY_STRING("&size="))
          .append(Strings::ToString(size));

  return result;
}

const ScreenScraperUser* RecalboxEndPoints::GetDirectUserObject() const
{
  static ScreenScraperUser sUser(ScraperSeamless::sScrapingEngineCount);

  return &sUser;
}

void RecalboxEndPoints::AddQueryParametersToMediaRequest(const FileData* game, long long size, std::string& url)
{
  url.append(BuildQueryString(game, size));
}

RecalboxEndPoints::RecalboxEndPoints()
  : mUUID(Strings::Trim(Files::LoadFile(Path(Upgrade::sLocalUUID)), " \t\r\n"))
  , mBoard()
  , mVersion(Strings::Trim(Files::LoadFile(Path(Upgrade::sLocalVersionFile)), " \t\r\n"))
  , mServerIndex(0)
  , mErrors(0)
{
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::UndetectedYet:        mBoard = "undetected"; break;
    case BoardType::Unknown:              mBoard = "unknown"; break;
    case BoardType::Pi0:                  mBoard = "RPi zero"; break;
    case BoardType::Pi02:                 mBoard = "RPi zero 2"; break;
    case BoardType::Pi1:                  mBoard = "RPi 1"; break;
    case BoardType::Pi2:                  mBoard = "RPi 2"; break;
    case BoardType::Pi3:                  mBoard = "RPi 3"; break;
    case BoardType::Pi3plus:              mBoard = "RPi 3B+"; break;
    case BoardType::Pi4:                  mBoard = "RPi 4"; break;
    case BoardType::Pi400:                mBoard = "RPi 400"; break;
    case BoardType::UnknownPi:            mBoard = "unknown RPi"; break;
    case BoardType::OdroidAdvanceGo:      mBoard = "Odroid GO Advance"; break;
    case BoardType::OdroidAdvanceGoSuper: mBoard = "Odroid GO Super"; break;
    case BoardType::PCx86:                mBoard = "x86"; break;
    case BoardType::PCx64:                mBoard = "x64"; break;
  }

  std::string servers = mDns.GetARecord(sRootDomainName);
  if (!servers.empty())
    mServers = Strings::Split(servers, '|', false);
}

std::string RecalboxEndPoints::GetUrlBase()
{
  if (mServers.empty()) return sRootDomainName;
  return mServers[mServerIndex];
}

void RecalboxEndPoints::NotifyError()
{
  Mutex::AutoLock lock(mErrorLocker);
  if (++mErrors >= 5)
    if (++mServerIndex >= (int)mServers.size())
      mServerIndex = 0;
}

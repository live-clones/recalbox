//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "ScreenScraperEndPoints.h"
#include "EmulationStation.h"
#include "utils/Strings.h"
#include "systems/SystemData.h"

std::string ScreenScraperEndPoints::BuildUrlCommon(ScreenScraperEndPoints::Api api, const std::string& login, const std::string& password)
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
  result.append("&softname=").append(Strings::URLEncode("Emulationstation-Recalbox-" + Strings::Trim(PROGRAM_VERSION_STRING)));
  // Credentials
  result.append("&ssid=").append(login);
  result.append("&sspassword=").append(password);

  return result;
}

std::string ScreenScraperEndPoints::GetUserInfoUrl(const std::string& login, const std::string& password)
{
  return BuildUrlCommon(Api::UserInfo, login, password);
}

std::string ScreenScraperEndPoints::GetGameInfoUrl(const std::string& login, const std::string& password, const FileData& game,
                                                   const std::string& crc32, const std::string& md5, long long int size)
{
  // Build the common part
  std::string result(BuildUrlCommon(Api::GameInfo, login, password));

  // Add gameinfo properties
  result.append("&romtype=rom");
  result.append("&systemeid=").append(Strings::ToString(game.System().Descriptor().ScreenScaperID()));
  result.append("&romnom=").append(Strings::URLEncode(game.FilePath().Filename()));
  result.append("&romtaille=").append(Strings::ToString(size));
  if (!crc32.empty())
    result.append("&crc=").append(crc32);
  if (!md5.empty())
    result.append("&md5=").append(md5);

  return result;
}

std::string ScreenScraperEndPoints::XOrTheSpaceSheriff(const std::string& _input, const std::string& key)
{
  std::string buffer = _input;
  for (int i = (int) _input.size(); --i >= 0;)
    buffer[i] = (char) ((unsigned char)_input[i] ^ (unsigned char)(key[i % key.size()] + (i * 17)));
  return buffer;
}


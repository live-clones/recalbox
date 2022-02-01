//
// Created by bkg2k on 03/04/2020.
//

#include <pistache/include/pistache/router.h>
#include <utils/Log.h>
#include <emulators/EmulatorList.h>
#include <systems/SystemDeserializer.h>
#include <rapidjson/document.h>
#include <utils/Files.h>
#include "RequestHandlerTools.h"
#include "Mime.h"
#include <LibretroRatio.h>
#include <utils/Zip.h>

using namespace Pistache;

JSONBuilder RequestHandlerTools::BuildPartitionObject(const DeviceInfo& info, const std::string& recalboxtype)
{
  JSONBuilder result;
  result.Open()
        .Field("recalbox", recalboxtype)
        .Field("devicebus", info.Bus)
        .Field("devicetype", info.Type)
        .Field("devicemodel", info.Model)
        .Field("mount", info.Mount)
        .Field("filesystem", info.FileSystem)
        .Field("filesystemtype", info.FileSystemType)
        .Field("size", info.Size << 10)
        .Field("used", info.Used << 10)
        .Close();
  return result;
}

void RequestHandlerTools::GetDevicePropertiesOf(DeviceInfo& info)
{
  // Get info from udevadm
  Strings::Vector strings = RequestHandlerTools::OutputLinesOf(std::string("/sbin/udevadm info --query=property --name=").append(info.FileSystem));
  for(const std::string& string : strings)
  {
    size_t pos = string.find('=');
    if (pos == std::string::npos) continue;
    if (Strings::StartsWith(string, LEGACY_STRING("ID_BUS=")))
      info.Bus = string.substr(pos + 1);
    else if (Strings::StartsWith(string, LEGACY_STRING("ID_VENDOR=")))
      info.Model.insert(0, string.substr(pos + 1) + ' ');
    else if (Strings::StartsWith(string, LEGACY_STRING("ID_MODEL=")))
      info.Model.append(string.substr(pos + 1));
    else if (Strings::StartsWith(string, LEGACY_STRING("ID_TYPE=")))
      info.Type = string.substr(pos + 1);
    else if (Strings::StartsWith(string, LEGACY_STRING("ID_NAME=")))
      info.Model = string.substr(pos + 1);
    else if (Strings::StartsWith(string, LEGACY_STRING("ID_FS_TYPE=")))
      info.FileSystemType = string.substr(pos + 1);
  }
  // Try to get missing info
  if (Strings::StartsWith(info.Mount, "/dev/mmcblk"))
  {
    info.Bus = "sdio";
    info.Type = "sd";
  }
  Strings::ReplaceAllIn(info.Model, '_', " ", 1);
}

void RequestHandlerTools::GetSystemResourcePath(Path& regionPath, Path& basePath, const std::string& system, const std::string& region, const char* resourceFileName)
{
  std::string regionString(sSystemResourceRegionPath);
  Strings::ReplaceAllIn(regionString, "%SYSTEM%", system);
  Strings::ReplaceAllIn(regionString, "%REGION%", region);
  Strings::ReplaceAllIn(regionString, "%FILE%", resourceFileName);
  regionPath = regionString;

  std::string baseString(sSystemResourceBasePath);
  Strings::ReplaceAllIn(baseString, "%SYSTEM%", system);
  Strings::ReplaceAllIn(baseString, "%FILE%", resourceFileName);
  basePath = baseString;
}

JSONBuilder RequestHandlerTools::SerializeBiosToJSON(const Bios& bios)
{
  return JSONBuilder().Open()
                      .Field("mandatory", bios.IsMandatory())
                      .Field("hashMatchingMandatory", bios.IsHashMatchingMandatory())
                      .Field("displayFileName", bios.Filename(false))
                      .Field("notes", bios.Notes())
                      .Field("currentMd5", bios.CurrentMD5())
                      .Field("md5List", bios.MD5List())
                      .Field("cores", Strings::Split(bios.Cores(), ','))
                      .Field("lightStatus", bios.LightStatusAsString())
                      .Field("realStatus", bios.BiosStatusAsString())
                      .Close();
}

JSONBuilder RequestHandlerTools::SerializeBiosListToJSON(const BiosList& biosList)
{
  JSONBuilder systemJson;
  systemJson.Open()
            .Field("fullName", biosList.FullName())
            .OpenObject("scanResult")
            .Field("total", biosList.BiosCount())
            .Field("ok", biosList.TotalBiosOk())
            .Field("ko", biosList.TotalBiosKo())
            .Field("unsafe", biosList.TotalBiosUnsafe())
            .Field("notFound", biosList.TotalFileNotFound())
            .Field("hashOk", biosList.TotalHashMatching())
            .Field("hashKo", biosList.TotalHashNotMatching())
            .CloseObject()
            .OpenObject("biosList");
  for(int biosIndex = biosList.BiosCount(); --biosIndex >= 0; )
  {
    const Bios& bios = biosList.BiosAt(biosIndex);
    systemJson.Field(bios.Filepath().ToChars(), SerializeBiosToJSON(bios));
  }
  systemJson.CloseObject()
            .Close();

  return systemJson;
}

std::string RequestHandlerTools::OutputOf(const std::string& command)
{
  std::string output;
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe != nullptr)
  {
    char buffer[1024];
    while (feof(pipe) == 0)
      if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        output.append(buffer);
    pclose(pipe);
  }
  return output;
}

Strings::Vector RequestHandlerTools::OutputLinesOf(const std::string& command)
{
  return Strings::Split(OutputOf(command), '\n');
}

void RequestHandlerTools::Error404(Http::ResponseWriter& response)
{
  LOG(LogError) << "Error 404";
  SetHeaders(response);
  Send(response, Http::Code::Not_Found, "404 - Not found", Mime::PlainText);
}

JSONBuilder RequestHandlerTools::SerializeEmulatorsAndCoreToJson(const EmulatorList& emulatorlist)
{
  JSONBuilder emulators;
  emulators.Open();
  for(int i = emulatorlist.Count(); --i >= 0; )
  {
    const EmulatorDescriptor& emulator = emulatorlist.EmulatorAt(i);
    JSONBuilder CoreJson;
    CoreJson.Open();
    for(int c = emulator.CoreCount(); --c >= 0; )
      CoreJson.Field(Strings::ToString((int)emulator.CorePriorityAt(c)).c_str(), emulator.CoreAt(c).mName);
    CoreJson.Close();
    emulators.Field(emulator.Name().c_str(), CoreJson);
  }
  emulators.Close();
  return emulators;
}

void RequestHandlerTools::LogRoute(const Pistache::Rest::Request& request, const char* methodName)
{
  LOG(LogDebug) << methodName << ": " << MethodToString(request.method()) << ' ' << request.resource();
}

const char* RequestHandlerTools::MethodToString(Pistache::Http::Method method)
{
  switch(method)
  {
    case Http::Method::Options: return "OPTIONS";
    case Http::Method::Get: return "GET";
    case Http::Method::Post: return "POST";
    case Http::Method::Head: return "HEAD";
    case Http::Method::Put: return "PUT";
    case Http::Method::Patch: return "PATCH";
    case Http::Method::Delete: return "DELETE";
    case Http::Method::Trace: return "TRACE";
    case Http::Method::Connect: return "CONNECT";
  }
  return "UNKNOWN";
}

/*
 * Send
 */


void RequestHandlerTools::SetHeaders(Http::ResponseWriter& response)
{
  response.headers().add<Pistache::Http::Header::AccessControlAllowMethods>("GET, POST, OPTIONS, DELETE");
  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.headers().add<Pistache::Http::Header::AccessControlAllowHeaders>("*");
}

void
RequestHandlerTools::Send(Pistache::Http::ResponseWriter& response, Pistache::Http::Code code, const std::string& body,
                          const Pistache::Http::Mime::MediaType& mime)
{
  SetHeaders(response);
  response.send(code, body, mime);
}

void RequestHandlerTools::Send(Pistache::Http::ResponseWriter& response, Pistache::Http::Code code)
{
  SetHeaders(response);
  response.send(code, nullptr, 0, Pistache::Http::Mime::MediaType());
}

void RequestHandlerTools::SendResource(const Path& resourcepath, Http::ResponseWriter& response, const Pistache::Http::Mime::MediaType& mimeType)
{
  if (resourcepath.Exists())
  {
    SetHeaders(response);
    Http::serveFile(response, resourcepath.ToString(), mimeType);
  }
  else
    Error404(response);
}

void RequestHandlerTools::SendResource(const Path& preferedpath, const Path& fallbackpath, Http::ResponseWriter& response, const Http::Mime::MediaType& mimetype)
{
  if (preferedpath.Exists())
  {
    SetHeaders(response);
    Http::serveFile(response, preferedpath.ToString(), mimetype);
  }
  else if (fallbackpath.Exists())
  {
    SetHeaders(response);
    Http::serveFile(response, fallbackpath.ToString(), mimetype);
  }
  else
    Error404(response);
}

const HashMap<std::string, Validator>& RequestHandlerTools::SelectConfigurationKeySet(const std::string& _namespace)
{
  enum class Namespace
  {
    Unknown,
    System,
    EmulationStation,
    Scraper,
    Kodi,
    Hyperion,
    Audio,
    Wifi,
    Controllers,
    Updates,
    Global,   //<! All emulators settings
    Specific, //<! Specific emulators settings
  };

  static HashMap<std::string, Namespace> sConverter
  ({
     { "system", Namespace::System },
     { "emulationstation", Namespace::EmulationStation },
     { "scraper", Namespace::Scraper },
     { "kodi", Namespace::Kodi },
     { "hyperion", Namespace::Hyperion },
     { "audio", Namespace::Audio },
     { "wifi", Namespace::Wifi },
     { "controllers", Namespace::Controllers },
     { "updates", Namespace::Updates },
     { "global", Namespace::Global },
     { "specific", Namespace::Global },
  });

  Namespace* pns = sConverter.try_get(_namespace);
  Namespace ns = (pns != nullptr) ? *pns : Namespace::Unknown;

  switch(ns)
  {
    case Namespace::System:
    {
      static HashMap<std::string, Validator> sList
      ({
         { "fbcp.enabled"            , Validator(true) },
         { "splash.length"           , Validator(-1, INT32_MAX) },
         { "manager.enabled"         , Validator(true) },
         { "security.enabled"        , Validator(true) },
         { "api.enabled"             , Validator(true) },
         { "es.videomode"            , Validator(GetAvailableResolutions(), false) },
         { "emulators.specialkeys"   , Validator(false, { "default", "nomenu", "none" }) },
         { "hostname"                , Validator("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") },
         { "samba.enabled"           , Validator(true) },
         { "virtual-gamepads.enabled", Validator(true) },
         { "ssh.enabled"             , Validator(true) },
         { "language"                , Validator(GetAvailableLanguages(), false) },
         { "kblayout"                , Validator(GetAvailableKeyboardLayout(), false) },
         { "timezone"                , Validator(GetAvailableTimeZone(), false) },
       });

      return sList;
    }
    case Namespace::EmulationStation:
    {
      static HashMap<std::string, Validator> sList
      ({
         { "menu"                   , Validator(false, { "default", "bartop", "none" }) },
         { "selectedsystem"         , Validator(GetSupportedSystemList(), false) },
         { "bootongamelist"         , Validator(true) },
         { "hidesystemview"         , Validator(true) },
         { "gamelistonly"           , Validator(true) },
         { "forcebasicgamelistview" , Validator(true) },
         { "filteradultgames"       , Validator(true) },
         { "collection.allgames"    , Validator(true) },
         { "collection.multiplayers", Validator(true) },
         { "collection.lastplayed"  , Validator(true) },
         { "videosnaps.delay"       , Validator(0, INT32_MAX) },
         { "videosnaps.loop"        , Validator(0, INT32_MAX) },
       });

      return sList;
    }
    case Namespace::Scraper:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "extractregionfromfilename", Validator(true) },
        { "getnamefrom"              , Validator(0, 2) },
        { "screenscraper.region"     , Validator(false, { "eu", "us", "jp", "wor" }) },
        { "screenscraper.language"   , Validator(false, { "en", "es", "pt", "fr", "de", "it", "nl", "ja", "zh", "ko", "ru", "da", "fi", "sv", "hu", "no", "pl", "cz", "sk", "tr" }) },
        { "screenscraper.media"      , Validator(false, { "screenshot", "title", "logo", "marquee", "box2d", "box3d", "mixv1", "mixv2" }) },
        { "screenscraper.thumbnail"  , Validator(false, { "screenshot", "title", "logo", "marquee", "box2d", "box3d", "mixv1", "mixv2" }) },
        { "screenscraper.video"      , Validator(false, { "none", "original", "normalized" }) },
        { "screenscraper.manual"     , Validator(true) },
        { "screenscraper.maps"       , Validator(true) },
        { "screenscraper.user"       , Validator() },
        { "screenscraper.password"   , Validator() },
      });

      return sList;
    }
    case Namespace::Kodi:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "enabled"         , Validator(true) },
        { "atstartup"       , Validator(true) },
        { "xbutton"         , Validator(true) },
        { "videomode"       , Validator(GetAvailableResolutions(), false) },
        { "network.waitmode", Validator(false, { "required", "wish", "nonce" }) },
        { "network.waittime", Validator(0, INT32_MAX) },
        { "network.waithost", Validator("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") },
      });

      return sList;
    }
    case Namespace::Hyperion:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "enabled"         , Validator(true) },
      });

      return sList;
    }
    case Namespace::Wifi:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "enabled", Validator(true) },
        { "region" , Validator(false, { "US", "CA", "JP", "DE", "NL", "IT", "PT", "LU", "NO", "FI", "DK", "CH", "CZ", "ES", "GB", "KR", "CN", "FR", "HK", "SG", "TW", "BR", "IL", "SA", "LB", "AE", "ZA", "AR", "AU", "AT", "BO", "CL", "GR", "IS", "IN", "IE", "KW", "LI", "LT", "MX", "MA", "NZ", "PL", "PR", "SK", "SI", "TH", "UY", "PA", "RU", "KW", "LI", "LT", "MX", "MA", "NZ", "PL", "PR", "SK", "SI", "TH", "UY", "PA", "RU", "EG", "TT", "TR", "CR", "EC", "HN", "KE", "UA", "VN", "BG", "CY", "EE", "MU", "RO", "CS", "ID", "PE", "VE", "JM", "BH", "OM", "JO", "BM", "CO", "DO", "GT", "PH", "LK", "SV", "TN", "PK", "QA", "DZ" }) },
        { "ssid"   , Validator() },
        { "key"    , Validator() },
        { "ip"     , Validator("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") },
        { "gateway", Validator("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") },
        { "netmask", Validator("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") },
      });

      return sList;
    }
    case Namespace::Audio:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "device" , Validator(GetAvailableSoundDevices(), false) },
        { "volume" , Validator(0, 100) },
        { "bgmusic", Validator(true) },
      });

      return sList;
    }
    case Namespace::Controllers:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "bluetooth.enabled", Validator(true) },
        { "bluetooth.ertm"   , Validator(true) },
        { "ps3.enabled"      , Validator(true) },
        { "ps3.driver"       , Validator(false, { "bluez", "official", "shanwan" }) },
        { "gpio.enabled"     , Validator(true) },
        { "gpio.args"        , Validator() },
        { "steam.enabled"    , Validator(true) },
        { "db9.enabled"      , Validator(true) },
        { "db9.args"         , Validator() },
        { "gamecon.enabled"  , Validator(true) },
        { "gamecon.args"     , Validator() },
        { "xarcade.enabled"  , Validator(true) },
      });

      return sList;
    }
    case Namespace::Updates:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "enabled"   , Validator(true) },
        { "type"      , Validator(false, std::vector<const char*>({ "stable", "review" })) },
        { "review.key", Validator() },
      });

      return sList;
    }
    case Namespace::Global:
    {
      static HashMap<std::string, Validator> sList
      ({
        { "videomode"                 , Validator(GetAvailableResolutions(), false) },
        { "shaderset"                 , Validator({ "none", "scanlines", "retro", "custom" }, false) },
        { "shaderset.file"            , Validator(GetAvailableShaders(), false) },
        { "integerscale"              , Validator(true) },
        { "shaders"                   , Validator(true) },
        { "ratio"                     , Validator(GetSupportedRatioList(), false) },
        { "smooth"                    , Validator(true) },
        { "rewind"                    , Validator(true) },
        { "autosave"                  , Validator(true) },
        { "retroachievements"         , Validator(true) },
        { "retroachievements.hardcore", Validator(true) },
        { "retroachievements.username", Validator() },
        { "retroachievements.password", Validator() },
        { "inputdriver"               , Validator(false, { "auto", "udev", "sdl2"}) },
        { "configfile"                , Validator() },
        { "demo.systemlist"           , Validator(GetSupportedSystemList(), true) },
        { "demo.duration"             , Validator(30, 600) },
        { "demo.infoscreenduration"   , Validator(5, 30) },
        { "translate"                 , Validator(true) },
        { "translate.from"            , Validator(false, {"auto", "EN", "ES", "FR", "IT", "DE", "JP", "NL", "CS", "DA", "SV", "HR", "KO", "ZH_CN", "ZH_TW", "CA", "BG", "BN", "EU", "AZ", "AR", "SQ", "AF", "EO", "ET", "TL", "FI", "GL", "KA", "EL", "GU", "HT", "IW", "HI", "HU", "IS", "ID", "GA", "KN", "LA", "LV", "LT", "MK", "MS", "MT", "NO", "FA", "PL", "PT", "RO", "RU", "SR", "SK", "SL", "SW", "TA", "TE", "TH", "TR", "UK", "UR", "VI", "CY", "YI"}) },
        { "translate.to"              , Validator(false, {"auto", "EN", "ES", "FR", "IT", "DE", "JP", "NL", "CS", "DA", "SV", "HR", "KO", "ZH_CN", "ZH_TW", "CA", "BG", "BN", "EU", "AZ", "AR", "SQ", "AF", "EO", "ET", "TL", "FI", "GL", "KA", "EL", "GU", "HT", "IW", "HI", "HU", "IS", "ID", "GA", "KN", "LA", "LV", "LT", "MK", "MS", "MT", "NO", "FA", "PL", "PT", "RO", "RU", "SR", "SK", "SL", "SW", "TA", "TE", "TH", "TR", "UK", "UR", "VI", "CY", "YI"}) },
        { "translate.apikey"          , Validator() },
        { "translate.url"             , Validator() },
        { "arcade"                    , Validator(true) },
        { "arcade.position"           , Validator(-200, 200) },
        { "arcade.includeneogeo"      , Validator(true) },
        { "arcade.hideoriginals"      , Validator(true) },
        { "netplay"                   , Validator(true) },
        { "netplay.nickname"          , Validator() },
        { "netplay.port"              , Validator(1, 65535) },
        { "netplay.relay"             , Validator() },
        { "netplay.systems"           , Validator() },
        { "netplay.lobby"             , Validator() },
      });

      return sList;
    }
    case Namespace::Specific:
    {
      static HashMap<std::string, Validator> sList
        ({
           { "videomode"                 , Validator(GetAvailableResolutions(), false) },
           { "shaderset"                 , Validator({ "none", "scanlines", "retro", "custom" }, false) },
           { "shaderset.file"            , Validator(GetAvailableShaders(), false) },
           { "integerscale"              , Validator(true) },
           { "shaders"                   , Validator(true) },
           { "ratio"                     , Validator(GetSupportedRatioList(), false) },
           { "smooth"                    , Validator(true) },
           { "rewind"                    , Validator(true) },
           { "autosave"                  , Validator(true) },
           { "retroachievements"         , Validator(true) },
           { "retroachievements.hardcore", Validator(true) },
           { "retroachievements.username", Validator() },
           { "retroachievements.password", Validator() },
           { "inputdriver"               , Validator(false, { "auto", "udev", "sdl2"}) },
           { "configfile"                , Validator() },
           { "translate"                 , Validator(true) },
           { "translate.from"            , Validator(false, {"auto", "EN", "ES", "FR", "IT", "DE", "JP", "NL", "CS", "DA", "SV", "HR", "KO", "ZH_CN", "ZH_TW", "CA", "BG", "BN", "EU", "AZ", "AR", "SQ", "AF", "EO", "ET", "TL", "FI", "GL", "KA", "EL", "GU", "HT", "IW", "HI", "HU", "IS", "ID", "GA", "KN", "LA", "LV", "LT", "MK", "MS", "MT", "NO", "FA", "PL", "PT", "RO", "RU", "SR", "SK", "SL", "SW", "TA", "TE", "TH", "TR", "UK", "UR", "VI", "CY", "YI"}) },
           { "translate.to"              , Validator(false, {"auto", "EN", "ES", "FR", "IT", "DE", "JP", "NL", "CS", "DA", "SV", "HR", "KO", "ZH_CN", "ZH_TW", "CA", "BG", "BN", "EU", "AZ", "AR", "SQ", "AF", "EO", "ET", "TL", "FI", "GL", "KA", "EL", "GU", "HT", "IW", "HI", "HU", "IS", "ID", "GA", "KN", "LA", "LV", "LT", "MK", "MS", "MT", "NO", "FA", "PL", "PT", "RO", "RU", "SR", "SK", "SL", "SW", "TA", "TE", "TH", "TR", "UK", "UR", "VI", "CY", "YI"}) },
           { "translate.apikey"          , Validator() },
           { "translate.url"             , Validator() },
         });

      return sList;
    }
    case Namespace::Unknown:
    default: break;
  }

  static HashMap<std::string, Validator> sList;
  return sList;
}

IniFile RequestHandlerTools::LoadConfiguration()
{
  return IniFile(Path(sConfiguration), false);
}

void RequestHandlerTools::SaveConfiguration(IniFile& configuration)
{
  configuration.Save();
}

void RequestHandlerTools::GetKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, Http::ResponseWriter& response)
{
  IniFile configuration = RequestHandlerTools::LoadConfiguration();
  JSONBuilder result;
  result.Open();
  for(const auto& key : keys)
  {
    std::string k(domain); k.append(1, '.'); k.append(key.first);
    JSONBuilder value;
    value.Open()
         .Field("exist", configuration.Exists(k));
    switch (key.second.Type())
    {
      case Validator::Types::StringFree:
      case Validator::Types::StringConstrained:
      case Validator::Types::StringMultiPicker:
      case Validator::Types::StringPicker: value.Field("value", configuration.AsString(k)); break;
      case Validator::Types::IntRange: value.Field("value", configuration.AsInt(k, 0)); break;
      case Validator::Types::Bool: value.Field("value", configuration.AsBool(k, false)); break;
      default: LOG(LogError) << "Unknown type " << (int) key.second.Type() << " for " << key.first;
    }
    value.Close();
    result.Field(key.first.c_str(), value);
  }
  result.Close();
  RequestHandlerTools::Send(response, Http::Code::Ok, result, Mime::Json);
}

void RequestHandlerTools::GetKeyValueOptions(const HashMap<std::string, Validator>& keys, Http::ResponseWriter& response)
{
  IniFile configuration = RequestHandlerTools::LoadConfiguration();
  JSONBuilder result;
  result.Open();
  for(const auto& key : keys)
  {
    JSONBuilder properties;
    properties.Open()
              .Field("type", key.second.TypeAsString());
    switch (key.second.Type())
    {
      case Validator::Types::StringFree: break;
      case Validator::Types::StringConstrained: properties.Field("allowedChars", key.second.StringConstraint()); break;
      case Validator::Types::StringPicker:
      case Validator::Types::StringMultiPicker: properties.Field("allowedStringList", key.second.StringList()); break;
      case Validator::Types::IntRange: properties.Field("lowerValue", key.second.Lower()).Field("higherValue", key.second.Higher()); break;
      case Validator::Types::Bool: break;
      default: LOG(LogError) << "Unknown type " << (int) key.second.Type() << " for " << key.first;
    }
    if (key.second.HasDisplay())
      properties.Field("displayableStringList", key.second.DisplayList());
    properties.Close();
    result.Field(key.first.c_str(), properties);
  }
  result.Close();
  RequestHandlerTools::Send(response, Http::Code::Ok, result, Mime::Json);
}

void RequestHandlerTools::SetKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, const std::string& jsonString, Http::ResponseWriter& response)
{
  rapidjson::Document json;
  json.Parse(jsonString.c_str());
  if (!json.HasParseError())
  {
    IniFile configuration = RequestHandlerTools::LoadConfiguration();
    bool changed = false;
    if (json.GetType() == rapidjson::Type::kObjectType)
      for(const auto& key : json.GetObject())
      {
        std::string shortKey(key.name.GetString());
        std::string value;
        switch(key.value.GetType())
        {
          case rapidjson::kFalseType: value = "0"; break;
          case rapidjson::kTrueType: value = "1"; break;
          case rapidjson::kNullType:
          case rapidjson::kObjectType:
          case rapidjson::kArrayType:
          {
            RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Key '" + shortKey + "' has un invalid value!", Mime::PlainText);
            return;
          }
          case rapidjson::kStringType: value = key.value.GetString(); break;
          case rapidjson::kNumberType: value = Strings::ToString(key.value.GetInt()); break;
        }
        Validator* validator = keys.try_get(shortKey);
        if (validator != nullptr)
        {
          if (validator->Validate(value))
          {
            std::string longKey(domain); longKey.append(1, '.'); longKey.append(shortKey);
            configuration.SetString(longKey, value);
            changed = true;
          }
          else
          {
            RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Key '" + shortKey + "' has un invalid value!", Mime::PlainText);
            return;
          }
        }
        else
        {
          RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Unknown Key: " + shortKey, Mime::PlainText);
          return;
        }
      }
    if (changed)
      RequestHandlerTools::SaveConfiguration(configuration);
    GetKeyValues(domain, keys, response);
    return;
  }
  RequestHandlerTools::Send(response, Http::Code::Bad_Request, "JSON Parsing error", Mime::PlainText);
}

void RequestHandlerTools::DeleteKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, const std
::string& jsonString, Http::ResponseWriter& response)
{
  rapidjson::Document json;
  json.Parse(jsonString.c_str());
  if (!json.HasParseError())
  {
    IniFile configuration = RequestHandlerTools::LoadConfiguration();
    bool changed = false;
    if (json.GetType() == rapidjson::Type::kArrayType)
      for(const auto& key : json.GetArray())
      {
        std::string shortKey(key.GetString());
        if (keys.contains(shortKey))
        {
          std::string longKey(domain); longKey.append(1, '.'); longKey.append(shortKey);
          configuration.Delete(longKey);
          changed = true;
        }
        else
        {
          RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Unknown Key: " + shortKey, Mime::PlainText);
          return;
        }
      }
    if (changed)
      RequestHandlerTools::SaveConfiguration(configuration);
    GetKeyValues(domain, keys, response);
    return;
  }
  RequestHandlerTools::Send(response, Http::Code::Bad_Request, "JSON Parsing error", Mime::PlainText);
}

const Strings::Vector& RequestHandlerTools::GetSupportedSystemList()
{
  static Strings::Vector result;

  if (result.empty())
  {
    SystemDeserializer deserializer;
    deserializer.LoadSystems();
    for(int i = deserializer.Count(); --i >= 0; )
    {
      SystemDescriptor descriptor;
      deserializer.Deserialize(i, descriptor);
      result.push_back(descriptor.Name());
    }
    // PATCH
    result.push_back("favorites");
  }

  return result;
}

const Strings::Vector& RequestHandlerTools::GetSupportedRatioList()
{
  static Strings::Vector result;

  if (result.empty())
    for(const auto& ratio : LibretroRatio::GetRatio())
      result.push_back(ratio.second);

  return result;
}

const std::string& RequestHandlerTools::GetArchitecture()
{
  static std::string sArchitecture(Files::LoadFile(Path("/recalbox/recalbox.arch")));
  return sArchitecture;
}

std::string RequestHandlerTools::GetCommandOutput(const std::string& command)
{
  std::string output;
  char buffer[10240];
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe != nullptr)
  {
    while (feof(pipe) == 0)
      if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        output.append(buffer);
    pclose(pipe);
  }
  return output;
}

HashMap<std::string, std::string> RequestHandlerTools::GetAvailableResolutions()
{
  static HashMap<std::string, std::string> sResolutions;

  if (sResolutions.empty())
  {
    std::string arch = GetArchitecture();

    sResolutions.insert_unique("default", "Default resolution");
    if (Strings::StartsWith(arch, "rpi", 3))
    {
      rapidjson::Document json;
      json.Parse(GetCommandOutput("tvservice -j -m CEA").c_str());
      if (!json.HasParseError())
        for(const auto& item : json.GetArray())
        {
          std::string name("CEA ");
          name.append(Strings::ToString(item["code"].GetInt()));
          std::string display(Strings::ToString(item["width"].GetInt()));
          display.append(1, 'x').append(Strings::ToString(item["height"].GetInt()))
                 .append(1, ' ')
                 .append(Strings::ToString(item["rate"].GetInt()))
                 .append("Hz ", 3)
                 .append(item["aspect_ratio"].GetString());
          if (item["scan"].GetString()[0] == 'i')
            display.append(" (Interlaced)");
          sResolutions.insert_unique(name + " HDMI", display + ", sound over HDMI");
          sResolutions.insert_unique(name + " DVI", display + ", no sound");
        }
      json.Parse(GetCommandOutput("tvservice -j -m DMT").c_str());
      if (!json.HasParseError())
        for(const auto& item : json.GetArray())
        {
          std::string name("DMT ");
          name.append(Strings::ToString(item["code"].GetInt()));
          std::string display(Strings::ToString(item["width"].GetInt()));
          display.append(1, 'x').append(Strings::ToString(item["height"].GetInt()))
                 .append(1, ' ')
                 .append(Strings::ToString(item["rate"].GetInt()))
                 .append("Hz ", 3)
                 .append(item["aspect_ratio"].GetString());
          if (item["scan"].GetString()[0] == 'i')
            display.append(" (Interlaced)");
          sResolutions.insert_unique(name + " HDMI", display + ", sound over HDMI");
          sResolutions.insert_unique(name + " DVI", display + ", no sound");
        }
    }
  }

  return sResolutions;
}

Strings::Vector RequestHandlerTools::GetAvailableSoundDevices()
{
  std::string output = GetCommandOutput("/recalbox/scripts/recalbox-config.sh lsaudio");
  return Strings::Split(output, '\n');
}

const Strings::Vector& RequestHandlerTools::GetAvailableTimeZone()
{
  static Strings::Vector output;

  if (output.empty())
  {
    std::string zones = Files::LoadFile(Path("/usr/share/zoneinfo/zone.tab"));
    Strings::Vector list = Strings::Split(zones, '\n');
    for (const std::string& line : list)
      if (!line.empty() && line[0] != '#')
      {
        int start = (int)line.find('\t');
        if (start == (int)std::string::npos) continue;
        start = (int)line.find('\t', start + 1);
        if (start == (int)std::string::npos) continue;
        int stop = (int)line.find('\t', ++start);
        std::string tz = line.substr(start, stop - start);
        output.push_back(tz);
      }
  }

  return output;
}

HashMap<std::string, std::string> RequestHandlerTools::GetAvailableLanguages()
{
  static HashMap<std::string, std::string> sLanguages
  ({
     { "eu_ES", "EUSKARA"       },
     { "zh_TW", "正體中文"          },
     { "zh_CN", "简体中文"          },
     { "de_DE", "DEUTSCH"       },
     { "en_US", "ENGLISH"       },
     { "es_ES", "ESPAÑOL"       },
     { "fr_FR", "FRANÇAIS"      },
     { "it_IT", "ITALIANO"      },
     { "pt_BR", "PORTUGUES"     },
     { "sv_SE", "SVENSKA"       },
     { "tr_TR", "TÜRKÇE"        },
     { "ca_ES", "CATALÀ"        },
     { "ar_YE", "اللغة العربية" },
     { "nl_NL", "NEDERLANDS"    },
     { "el_GR", "ελληνικά"      },
     { "ko_KR", "한국어"           },
     { "nn_NO", "NORSK"         },
     { "nb_NO", "BOKMAL"        },
     { "pl_PL", "POLSKI"        },
     { "ja_JP", "日本語"           },
     { "ru_RU", "Русский"       },
     { "hu_HU", "MAGYAR"        },
     { "cs_CZ", "čeština"       },
     { "lv_LV", "latviešu"      },
     { "lb_LU", "Lëtzebuergesch"},
  });

  return sLanguages;
}

HashMap<std::string, std::string> RequestHandlerTools::GetAvailableKeyboardLayout()
{
  static HashMap<std::string, std::string> kbLayouts;

  if (kbLayouts.empty())
  {
    static Path layouts("/usr/share/keymaps/i386");
    static std::string ext(".map.gz");
    for (const char* sub : { "azerty", "bepo", "qwerty", "qwertz" })
      for(const Path& filePath : (layouts / sub).GetDirectoryContent())
      {
        std::string fileName = filePath.Filename();
        if (Strings::EndsWith(fileName, ext))
        {
          fileName = fileName.substr(0, fileName.size() - ext.size());
          kbLayouts.insert_unique(std::string(fileName), std::string(sub) + ":" + fileName);
        }
      }
  }

  return kbLayouts;
}

void RequestHandlerTools::GetAvailableShadersIn(const Path& rootPath, Strings::Vector& results)
{
  Path::PathList pathList = rootPath.GetDirectoryContent();
  for(const Path& path : pathList)
    if (path.IsDirectory())
      GetAvailableShadersIn(path, results);
    else if (path.IsFile())
      if (path.Extension() == ".glslp")
        results.push_back(path.ToString());
}

Strings::Vector RequestHandlerTools::GetAvailableShaders()
{
  Strings::Vector result;
  static Path shaderPath("/recalbox/share_init/shaders");

  GetAvailableShadersIn(shaderPath, result);
  int commonPartLength = (int)shaderPath.ToString().length() + 1;
  for(std::string& path : result)
    path.erase(0, commonPartLength);

  return result;
}

void RequestHandlerTools::GetEmbeddedBios(const Path& base, HashMap<std::string, bool>& results)
{
  // Read all embedded path. Bas must be /recalbox/share/ as bios is already in path of every line
  Strings::Vector pathlist = Strings::Split(Files::LoadFile(Path("/recalbox/share_init/embedded.list")), '\n');
  results.clear();
  for(const std::string& path : pathlist)
    results.insert_unique(std::string((base / path).ToString()), true);
}

void RequestHandlerTools::GetAvailableBios(const Path& rootPath, Path::PathList& results, const HashMap<std::string, bool>& embedded)
{
  Path::PathList pathList = rootPath.GetDirectoryContent();
  for(const Path& path : pathList)
    if (path.IsDirectory())
      GetAvailableBios(path, results, embedded);
    else if (path.IsFile())
      if (!embedded.contains(path.ToString()))
        results.push_back(path);
}

Path RequestHandlerTools::GetCompressedBiosFolder()
{
  Path baseFolder("/recalbox/share/bios");

  // Get embedded list
  HashMap<std::string, bool> embedded;
  GetEmbeddedBios(baseFolder.Directory(), embedded);

  // Get diff list embedded/user bios
  Path::PathList biosPath;
  GetAvailableBios(baseFolder, biosPath, embedded);

  Path destination("/recalbox/share/system/bios_backup.zip");
  {
    Zip zip(destination, true);
    for(const Path& path : biosPath)
      zip.Add(path, baseFolder);
  }

  return destination;
}

bool RequestHandlerTools::IsValidSystem(const std::string& system)
{
  Strings::Vector systemList = RequestHandlerTools::GetSupportedSystemList();
  for(const std::string& s : systemList)
    if (s == system)
      return true;

  return false;
}

bool
RequestHandlerTools::GetUploadedFile(const Rest::Request& request, std::string& filename, int& startOffset, int& size)
{
  #define GUF_DOUBLE_EOF "\r\n\r\n"
  #define GUF_BOUNDARY "boundary="
  #define GUF_FILENAME "filename=\""

  // Get barrier
  const auto* contentType = request.headers().tryGet<Http::Header::ContentType>().get();
  if (contentType == nullptr) return false;
  std::string boundary = contentType->mime().raw();
  size_t start = boundary.find(GUF_BOUNDARY);
  if (start == std::string::npos) return false;
  boundary.erase(0, start + (sizeof(GUF_BOUNDARY) - 1));
  boundary.insert(0, "--", 2);

  LOG(LogError) << "BOUNDARY: " << boundary;

  // Lookup filename
  start = request.body().find(boundary, 0);
  if (start != std::string::npos)
    for(;;)
    {
      // Find start/stop
      size_t end = request.body().find(boundary, start + boundary.size());
      if (end == std::string::npos) break;

      // Get EOL
      size_t headerStart = request.body().find('\n', start + boundary.size());
      if (headerStart != std::string::npos && headerStart < end)
      {
        // Get double EOL
        size_t headerStop = request.body().find(GUF_DOUBLE_EOF, ++headerStart);
        if (headerStop != std::string::npos && headerStop < end)
        {
          // locate filename
          size_t fileNameStart = request.body().find(GUF_FILENAME, headerStart);
          if (headerStart != std::string::npos && headerStart < headerStop)
          {
            fileNameStart += (sizeof(GUF_FILENAME) - 1);
            size_t fileNameStop = request.body().find('"', fileNameStart);
            if (fileNameStop != std::string::npos && fileNameStop < headerStop)
            {
              // Extact filename
              filename = request.body().substr(fileNameStart, fileNameStop - fileNameStart);
              // Store indexes
              startOffset = (int)(headerStop + sizeof(GUF_DOUBLE_EOF) - 1);
              size = (int)(end - startOffset) - 2 /* size of last EOF */;
              return true;
            }
          }
        }
      }
      // Restart from next boundary
      start = end;
    }

  #undef GUF_FILENAME
  #undef GUF_BOUNDARY
  #undef GUF_DOUBLE_EOF

  return false;
}

bool RequestHandlerTools::ExtractArray(const Rest::Request& request, Strings::Vector& result)
{
  rapidjson::Document json;
  json.Parse(request.body().c_str());
  if (!json.HasParseError())
    if (json.GetType() == rapidjson::Type::kArrayType)
    {
      for (const auto& key : json.GetArray())
        result.push_back(key.GetString());
      return true;
    }

  return false;
}


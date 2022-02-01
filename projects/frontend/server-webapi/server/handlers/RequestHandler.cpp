//
// Created by bkg2k on 30/03/2020.
//

#include <utils/Files.h>
#include <utils/Log.h>
#include <utils/json/JSONBuilder.h>
#include <systems/SystemDeserializer.h>
#include "RequestHandler.h"
#include "Mime.h"
#include "RequestHandlerTools.h"

using namespace Pistache;

void RequestHandler::FileServer(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "FileServer");

  Path path = mWWWRoot / (request.resource() != "/" ? request.resource() : mDefaultFile);
  std::string ext = path.Extension();

  bool knownMime = Mime::ExtToMIME.contains(ext);
  const Pistache::Http::Mime::MediaType& mimeType = knownMime ? Mime::ExtToMIME[ext] : Mime::BinaryFile;
  if (!knownMime)
    LOG(LogWarning) << "Unknown MIME Type for file extension: " << ext;
  RequestHandlerTools::SendResource(path, response, mimeType);
}

void RequestHandler::Versions(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "Versions");

  // Get libretro cores
  std::map<std::string, std::string> cores;
  Strings::Vector coreLines = Strings::Split(Files::LoadFile(Path("/recalbox/share/system/configs/retroarch.corenames")), '\n');
  for(const std::string& coreLine : coreLines)
  {
    Strings::Vector coreItems = Strings::Split(coreLine, ';');
    if (coreItems.size() == 3) cores[coreItems[0]] = coreItems[2];
  }
  std::string retroarchVersion = Strings::Extract(RequestHandlerTools::OutputOf("retroarch --version 2>&1"), "-- ", " --", 3, 3);

  // Get Recalbox & Linux version
  std::string recalboxVersion = Files::LoadFile(Path("/recalbox/recalbox.version"));
  std::string linuxVersion = Files::LoadFile(Path("/proc/version"));
  int p = linuxVersion.find('('); if (p != (int)std::string::npos) linuxVersion = Strings::Trim(linuxVersion.substr(0, p));

  // Build final version object
  JSONBuilder json;
  json.Open()
      .Field("webapi", "2.0")
      .Field("recalbox", recalboxVersion)
      .Field("linux", linuxVersion)
      .OpenObject("libretro")
        .Field("retroarch", retroarchVersion)
        .Field("cores", cores)
      .CloseObject()
      .Close();

  RequestHandlerTools::Send(response, Http::Code::Ok, json, Mime::Json);
}

void RequestHandler::SystemInfo(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "CpuInfo");

  JSONBuilder sysInfo;
  sysInfo.Open()
         .Field("timestamp", DateTime().ToISO8601())
         .Field("platform", mSysInfos.BuildPlatformObject())
         .Field("cpus", mSysInfos.BuildCpuObject(true))
         .Field("memory", mSysInfos.BuildMemoryObject(true))
         .Field("temperature", mSysInfos.BuildTemperatureObject(true))
         .Close();

  RequestHandlerTools::Send(response, Http::Code::Ok, sysInfo, Mime::Json);
}

void RequestHandler::StorageInfo(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "StorageInfo");

  Strings::Vector lines = RequestHandlerTools::OutputLinesOf("df -T");
  JSONBuilder result;
  result.Open()
        .OpenObject("storages");
  int id = 0;
  for(int i=lines.size(); --i>0; )
  {
    Strings::Vector parts = Strings::Split(lines[i], ' ', true);
    if (parts.size() == 7)
    {
      RequestHandlerTools::DeviceInfo info(parts[6], parts[0], parts[1], parts[2], parts[3]);
      RequestHandlerTools::GetDevicePropertiesOf(info);
      if (info.Mount == "/") result.Field(Strings::ToString(id++).data(), RequestHandlerTools::BuildPartitionObject(info, "system"));
      else if (info.Mount == "/boot") result.Field(Strings::ToString(id++).data(), RequestHandlerTools::BuildPartitionObject(info, "boot"));
      else if (info.Mount == "/recalbox/share") result.Field(Strings::ToString(id++).data(), RequestHandlerTools::BuildPartitionObject(info, "share"));
      else if (info.Mount == "/recalbox/share/bootvideos") ; // Filtered out
      else if (Strings::StartsWith(info.Mount, "/recalbox/share") &&
               Strings::StartsWith(info.FileSystem, "//")) result.Field(Strings::ToString(id++).data(), RequestHandlerTools::BuildPartitionObject(info, "network"));
      else result.Field(Strings::ToString(id++).data(), RequestHandlerTools::BuildPartitionObject(info, "unknown"));
    }
    else LOG(LogError) << "df -T unknown result : " << lines[i];
  }
  result.CloseObject()
        .Close();
  RequestHandlerTools::Send(response, Http::Code::Ok, result, Mime::Json);
}

void RequestHandler::BiosDownload(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "BiosDownload");

  Path path = RequestHandlerTools::GetCompressedBiosFolder();
  RequestHandlerTools::SendResource(path, response, Mime::Zip);
}

void RequestHandler::BiosUpload(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "BiosUpload");

  std::string fileName;
  int start = 0;
  int size = 0;
  if (RequestHandlerTools::GetUploadedFile(request, fileName, start, size))
  {
    const char* data = request.body().data() + start;

    // Get bios
    std::string biosMd5 = md5(data, size);

    // Load and scan
    if (mBiosManager.SystemCount() == 0)
      mBiosManager.LoadFromFile();
    mBiosManager.Scan(nullptr, true);

    // Lookup
    const Bios* bios = nullptr;
    BiosManager::LookupResult result = mBiosManager.Lookup(request.splatAt(0).name(), biosMd5, bios);

    std::string extraResult = "Unknown";
    switch (result)
    {
      case BiosManager::AlreadyExists:
      case BiosManager::Found:
      {
        bool ok = (result == BiosManager::Found) ? Files::SaveFile(bios->Filepath(), data, size) : true;
        if (ok)
          RequestHandlerTools::Send(response, Http::Code::Ok, RequestHandlerTools::SerializeBiosToJSON(*bios), Mime::Json);
        else
          RequestHandlerTools::Send(response, Http::Code::Insufficient_Storage);
        break;
      }
      case BiosManager::NotFound:
      default: RequestHandlerTools::Error404(response);
        break;
    }

    // Response
    JSONBuilder jsonResult;
    jsonResult.Open();
    jsonResult.CloseObject();

    RequestHandlerTools::Send(response, Http::Code::Ok);
  }
}

void RequestHandler::BiosGetAll(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "BiosGetAll");

  // Load and scan
  if (mBiosManager.SystemCount() == 0)
    mBiosManager.LoadFromFile();
  mBiosManager.Scan(nullptr, true);

  JSONBuilder output;
  output.Open();

  // Loop through systems
  for(int sysIndex = 0; sysIndex < mBiosManager.SystemCount(); sysIndex++)
  {
    const BiosList& list = mBiosManager.SystemBios(sysIndex);
    output.Field(list.Name().c_str(), RequestHandlerTools::SerializeBiosListToJSON(list));
  }
  output.Close();

  RequestHandlerTools::Send(response, Http::Code::Ok, output, Mime::Json);
}

void RequestHandler::BiosGetSystem(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "BiosGetSystem");

  // Load and scan
  if (mBiosManager.SystemCount() == 0)
    mBiosManager.LoadFromFile();
  mBiosManager.Scan(nullptr, true);

  std::string systemName = request.splatAt(0).name();
  const BiosList& list = mBiosManager.SystemBios(systemName);
  // Empty system does not have a name => not found
  if (list.Name() == systemName)
    RequestHandlerTools::Send(response, Http::Code::Ok, RequestHandlerTools::SerializeBiosListToJSON(list), Mime::Json);
  else
    RequestHandlerTools::Error404(response);
}

void RequestHandler::SystemsGetAll(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsGetAll");

  JSONBuilder systems;
  systems.Open()
         .Field("romPath", "/recalbox/share/roms")
         .OpenObject("systemList");

  SystemDeserializer deserializer;
  deserializer.LoadSystems();
  for(int i = 0; i < deserializer.Count(); ++i)
  {
    SystemDescriptor descriptor;
    deserializer.Deserialize(i, descriptor);

    JSONBuilder emulators = RequestHandlerTools::SerializeEmulatorsAndCoreToJson(descriptor.EmulatorTree());

    JSONBuilder systemJson;
    systemJson.Open()
              .Field("name", descriptor.Name())
              .Field("fullname", descriptor.FullName())
              .Field("romFolder", descriptor.RomPath().Filename())
              .Field("themeFolder", descriptor.ThemeFolder())
              .Field("extensions", Strings::Split(descriptor.Extension(), ' ', true))
              .Field("command", descriptor.Command())
              .Field("emulators", emulators)
              .Close();
    systems.Field(Strings::ToString(i).c_str(), systemJson);
  }

  systems.CloseObject()
         .Close();
  RequestHandlerTools::Send(response, Http::Code::Ok, systems, Mime::Json);
}

void RequestHandler::SystemsGetActives(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsGetActives");

  RequestHandlerTools::Send(response, Http::Code::Method_Not_Allowed);
}

void RequestHandler::SystemsResourceGetConsole(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsResourceGetConsole");

  Path first, second;
  RequestHandlerTools::GetSystemResourcePath(first, second, request.splatAt(0).name(), request.splatAt(1).name(), "console.svg");
  RequestHandlerTools::SendResource(first, second, response, Mime::ImageSvg);
}

void RequestHandler::SystemsResourceGetControler(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsResourceGetControler");

  Path first, second;
  RequestHandlerTools::GetSystemResourcePath(first, second, request.splatAt(0).name(), request.splatAt(1).name(), "controler.svg");
  RequestHandlerTools::SendResource(first, second, response, Mime::ImageSvg);
}

void RequestHandler::SystemsResourceGetControls(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsResourceGetControls");

  Path first, second;
  RequestHandlerTools::GetSystemResourcePath(first, second, request.splatAt(0).name(), request.splatAt(1).name(), "controls.svg");
  RequestHandlerTools::SendResource(first, second, response, Mime::ImageSvg);
}

void RequestHandler::SystemsResourceGetGame(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsResourceGetGame");

  Path first, second;
  RequestHandlerTools::GetSystemResourcePath(first, second, request.splatAt(0).name(), request.splatAt(1).name(), "game.svg");
  RequestHandlerTools::SendResource(first, second, response, Mime::ImageSvg);
}

void RequestHandler::SystemsResourceGetLogo(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemsResourceGetLogo");

  Path first, second;
  RequestHandlerTools::GetSystemResourcePath(first, second, request.splatAt(0).name(), request.splatAt(1).name(), "logo.svg");
  RequestHandlerTools::SendResource(first, second, response, Mime::ImageSvg);
}

void RequestHandler::ConfigurationGet(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "ConfigurationGet");

  std::string ns = request.splatAt(0).name();
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet(ns);
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::GetKeyValues(ns, keys, response);
}

void RequestHandler::ConfigurationOptions(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "ConfigurationOptions");

  std::string ns = request.splatAt(0).name();
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet(ns);
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::GetKeyValueOptions(keys, response);
}

void RequestHandler::ConfigurationSet(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "ConfigurationSet");

  std::string ns = request.splatAt(0).name();
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet(ns);
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::SetKeyValues(ns, keys, request.body(), response);
}

void RequestHandler::ConfigurationDelete(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "ConfigurationDelete");

  std::string ns = request.splatAt(0).name();
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet(ns);
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::DeleteKeyValues(ns, keys, request.body(), response);
}

void RequestHandler::SystemConfigurationGet(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemConfigurationGet");

  // Check system
  std::string subSystem = request.splatAt(0).name();
  if (!RequestHandlerTools::IsValidSystem(subSystem))
    RequestHandlerTools::Error404(response);

  // Check data
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet("specific");
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::GetKeyValues(subSystem, keys, response);
}

void RequestHandler::SystemConfigurationSet(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemConfigurationSet");

  // Check system
  std::string subSystem = request.splatAt(0).name();
  if (!RequestHandlerTools::IsValidSystem(subSystem))
    RequestHandlerTools::Error404(response);

  // Check data
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet("specific");
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::SetKeyValues(subSystem, keys, request.body(), response);
}

void RequestHandler::SystemConfigurationOptions(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemConfigurationOptions");

  // Check system
  std::string subSystem = request.splatAt(0).name();
  if (!RequestHandlerTools::IsValidSystem(subSystem))
    RequestHandlerTools::Error404(response);

  // Check data
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet("specific");
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::GetKeyValueOptions(keys, response);
}

void RequestHandler::SystemConfigurationDelete(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "SystemConfigurationDelete");

  // Check system
  std::string subSystem = request.splatAt(0).name();
  if (!RequestHandlerTools::IsValidSystem(subSystem))
    RequestHandlerTools::Error404(response);

  // Check data
  const HashMap<std::string, Validator>& keys = RequestHandlerTools::SelectConfigurationKeySet("specific");
  if (keys.empty())
    RequestHandlerTools::Error404(response);

  RequestHandlerTools::DeleteKeyValues(subSystem, keys, request.body(), response);
}

void RequestHandler::MediaUserGetList(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "MediaUserGetList");

  Path mediaPath("/recalbox/share/screenshots");
  Path::PathList list = mediaPath.GetDirectoryContent();

  static std::string imagesExtentions(".jpg|.jpeg|.png|.gif");
  static std::string videosExtentions(".mkv|.avi|.mp4");

  JSONBuilder result;
  result.Open()
        .Field("mediaPath", mediaPath.ToString())
        .OpenObject("mediaList");
  for(const Path& path : list)
  {
    bool ok = false;
    result.OpenObject(path.MakeRelative(mediaPath, ok).ToChars());
    std::string ext = Strings::ToLowerASCII(path.Extension());
    if (imagesExtentions.find(ext) != std::string::npos) result.Field("type", "image");
    else if (videosExtentions.find(ext) != std::string::npos) result.Field("type", "video");
    else result.Field("type", "unknown");
    result.CloseObject();
  }
  result.CloseObject()
        .Close();
  RequestHandlerTools::Send(response, Http::Code::Ok, result, Mime::Json);
}

void RequestHandler::MediaUserDelete(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "MediaUserDelete");

  Strings::Vector list;
  if (RequestHandlerTools::ExtractArray(request, list))
  {
    Path mediaPath("/recalbox/share/screenshots");
    for(const std::string& mediaFile : list)
    {
      Path media = mediaPath / mediaFile;
      if (media.Exists()) media.Delete();
      else RequestHandlerTools::Error404(response);
    }
    RequestHandlerTools::Send(response, Http::Code::Ok);
  }
  RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Invalid json array!", Mime::PlainText);
}

void RequestHandler::MediaGet(const Rest::Request& request, Http::ResponseWriter response)
{
  RequestHandlerTools::LogRoute(request, "MediaGameGet");

  // Get path
  Path path(Strings::Decode64(request.splatAt(0).name()));
  // Check extension
  std::string ext = Strings::ToLowerASCII(path.Extension()).append(1, '.');
  if (path.Exists())
  {
    if (path.IsFile())
    {
      // Image
      if (ext == ".gif")      RequestHandlerTools::SendResource(path, response, Mime::ImageGif);
      else if (ext == ".jpg") RequestHandlerTools::SendResource(path, response, Mime::ImageJpg);
      else if (ext == ".png") RequestHandlerTools::SendResource(path, response, Mime::ImagePng);
      else if (ext == ".svg") RequestHandlerTools::SendResource(path, response, Mime::ImageSvg);
      // Video
      else if (ext == ".mkv") RequestHandlerTools::SendResource(path, response, Mime::VideoMkv);
      else if (ext == ".mp4") RequestHandlerTools::SendResource(path, response, Mime::VideoMp4);
      else if (ext == ".avi") RequestHandlerTools::SendResource(path, response, Mime::VideoAvi);
      // Video
      else if (ext == ".pdf") RequestHandlerTools::SendResource(path, response, Mime::FilePdf);
      // Unknown
      else RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Invalid media extension!", Mime::PlainText);
    }
    else RequestHandlerTools::Send(response, Http::Code::Bad_Request, "Target is a directory!", Mime::PlainText);
  }
  else RequestHandlerTools::Error404(response);
}

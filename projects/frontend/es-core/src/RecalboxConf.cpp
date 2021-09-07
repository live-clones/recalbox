#include "RecalboxConf.h"
#include <utils/Files.h>
#include <usernotifications/NotificationManager.h>

static Path recalboxConfFile("/recalbox/share/system/recalbox.conf");
static Path recalboxConfFileInit("/recalbox/share_init/system/recalbox.conf");
constexpr const int RecalboxConf::sNetplayDefaultPort;

RecalboxConf::RecalboxConf()
  : IniFile(recalboxConfFile, recalboxConfFileInit),
    StaticLifeCycleControler<RecalboxConf>("RecalboxConf")
{
}

void RecalboxConf::OnSave()
{
  NotificationManager::Instance().Notify(Notification::ConfigurationChanged, recalboxConfFile.ToString());
}

std::string RecalboxConf::GetLanguage()
{
  std::string locale = Strings::ToLowerASCII(RecalboxConf::Instance().GetSystemLanguage());
  return (locale.length() == 5) ? locale.substr(0, 2) : "en";
}

std::string RecalboxConf::GetCountry()
{
  std::string locale = Strings::ToLowerASCII(RecalboxConf::Instance().GetSystemLanguage());
  return (locale.length() == 5) ? locale.substr(3, 2) : "us";
}

RecalboxConf::Menu RecalboxConf::MenuFromString(const std::string& menu)
{
  if (menu == "bartop") return Menu::Bartop;
  if (menu == "none") return Menu::None;
  return Menu::Default;
}

const std::string& RecalboxConf::MenuFromEnum(RecalboxConf::Menu menu)
{
  switch (menu)
  {
    case Menu::Bartop:
    {
      static std::string sBartop = "bartop";
      return sBartop;
    }
    case Menu::None:
    {
      static std::string sNone = "none";
      return sNone;
    }
    case Menu::Default:
    default: break;
  }
  static std::string sDefault = "default";
  return sDefault;
}

RecalboxConf::Relay RecalboxConf::RelayFromString(const std::string& relay)
{
  if (relay == "nyc") return Relay::NewYork;
  if (relay == "madrid") return Relay::Madrid;
  if (relay == "montreal") return Relay::Montreal;
  if (relay == "saopaulo") return Relay::Saopaulo;
  return Relay::None;
}

const std::string& RecalboxConf::RelayFromEnum(RecalboxConf::Relay relay)
{
  switch (relay)
  {
    case Relay::NewYork:  { static std::string sNewYork = "nyc"; return sNewYork; }
    case Relay::Madrid:   { static std::string sMadrid = "madrid"; return sMadrid; }
    case Relay::Montreal: { static std::string sMontreal = "montreal"; return sMontreal; }
    case Relay::Saopaulo: { static std::string sSauPaulo = "saopaulo"; return sSauPaulo; }
    case Relay::None: default: break;
  }
  static std::string sDefault = "none";
  return sDefault;
}

DefineSystemGetterSetterImplementation(Emulator, std::string, String, sSystemEmulator, "")
DefineSystemGetterSetterImplementation(Core, std::string, String, sSystemCore, "")
DefineSystemGetterSetterImplementation(Ratio, std::string, String, sSystemRatio, GetGlobalRatio())
DefineSystemGetterSetterImplementation(Smooth, bool, Bool, sSystemSmooth, GetGlobalSmooth())
DefineSystemGetterSetterImplementation(Rewind, bool, Bool, sSystemRewind, GetGlobalRewind())
DefineSystemGetterSetterImplementation(AutoSave, bool, Bool, sSystemAutoSave, GetGlobalAutoSave())
DefineSystemGetterSetterImplementation(Shaders, std::string, String, sSystemShaders, GetGlobalShaders())
DefineSystemGetterSetterImplementation(ShaderSet, std::string, String, sSystemShaderSet, GetGlobalShaderSet())

DefineEmulationStationSystemGetterSetterImplementation(FilterAdult, bool, Bool, sSystemFilterAdult, false)
DefineEmulationStationSystemGetterSetterImplementation(FlatFolders, bool, Bool, sSystemFlatFolders, false)
DefineEmulationStationSystemGetterSetterNumericEnumImplementation(Sort, FileSorts::Sorts, sSystemSort, FileSorts::Sorts::FileNameAscending)
DefineEmulationStationSystemGetterSetterNumericEnumImplementation(RegionFilter, Regions::GameRegions, sSystemRegionFilter, Regions::GameRegions::Unknown)

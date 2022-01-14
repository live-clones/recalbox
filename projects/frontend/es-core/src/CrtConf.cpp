//
// Created by bkg2k on 11/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <CrtConf.h>
#include <usernotifications/NotificationManager.h>

static Path crtConfFile("/boot/crt/recalbox-crt-options.cfg");

CrtConf::CrtConf()
  : IniFile(crtConfFile, true),
    StaticLifeCycleControler<CrtConf>("CrtConf")
{
}

void CrtConf::OnSave()
{
  NotificationManager::Instance().Notify(Notification::ConfigurationChanged, crtConfFile.ToString());
}

CrtAdapterType CrtConf::CrtAdapterFromString(const std::string& adapter)
{
  if (adapter == "recalboxrgbdual") return CrtAdapterType::RGBDual;
  if (adapter == "pi2scart"       ) return CrtAdapterType::Pi2Scart;
  if (adapter == "rgbpi"          ) return CrtAdapterType::RGBPi;
  if (adapter == "vga666"         ) return CrtAdapterType::Vga666;
  return CrtAdapterType::None;
}

const std::string& CrtConf::CrtAdapterFromEnum(CrtAdapterType adapter)
{
  switch(adapter)
  {
    case CrtAdapterType::RGBDual:  { static std::string adapterString("recalboxrgbdual"); return adapterString; }
    case CrtAdapterType::Pi2Scart: { static std::string adapterString("pi2scart"); return adapterString; }
    case CrtAdapterType::RGBPi:    { static std::string adapterString("rgbpi"); return adapterString; }
    case CrtAdapterType::Vga666:   { static std::string adapterString("vga666"); return adapterString; }
    case CrtAdapterType::None:
    default: break;
  }
  static std::string sDefault = "";
  return sDefault;
}
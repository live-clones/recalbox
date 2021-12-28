//
// Created by bkg2k on 20/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <hardware/crt/CrtAdapterDetector.h>
#include <utils/Files.h>
#include <RecalboxConf.h>
#include <hardware/crt/CrtNull.h>
#include <hardware/crt/CrtVga666.h>
#include <hardware/crt/CrtRGBPi.h>
#include <hardware/crt/CrtPi2Scart.h>
#include <hardware/crt/CrtRGBDual.h>

CrtAdapterType CrtAdapterDetector::DetectCrtAdapter(bool& automaticallyDetected)
{
  // Reset
  CrtAdapterType result = CrtAdapterType::None;
  automaticallyDetected = false;

  // RGB Dual hat
  std::string vendor = Files::LoadFile(Path(sHatVendorFile));
  std::string product = Files::LoadFile(Path(sHatProductFile));
  if (vendor == sRecalboxVendorString &&
      product == sRGBDualProductString)
  {
    result = CrtAdapterType::RGBDual;
    automaticallyDetected = true;
  }
  else
  {
    // User config
    result = RecalboxConf::Instance().GetSystemCRT();
  }

  return result;
}

ICrtInterface& CrtAdapterDetector::GetCrtBoard()
{
  bool automatic = false;
  switch(DetectCrtAdapter(automatic))
  {
    case CrtAdapterType::Vga666: return *(new CrtVga666(automatic));
    case CrtAdapterType::RGBPi: return *(new CrtRGBPi(automatic));
    case CrtAdapterType::Pi2Scart: return *(new CrtPi2Scart(automatic));
    case CrtAdapterType::RGBDual: return *(new CrtRGBDual(automatic));
    case CrtAdapterType::None:
    default: break;
  }

  return *(new CrtNull(automatic));
}

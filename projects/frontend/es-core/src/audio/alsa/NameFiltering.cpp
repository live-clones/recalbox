//
// Created by bkg2k on 14/08/2020.
//

#include "NameFiltering.h"
#include <utils/Strings.h>

std::string NameFiltering::Filter(const std::string& source)
{
  std::string result = source;

  // Pi3 replacement
  if (result == "bcm2835 ALSA") return "Headphones";
  if (result == "bcm2835 IEC958/HDMI") return "HDMI";

  // Pi4 filtering
  Strings::ReplaceAllIn(result, "bcm2835", "");

  // Generic HDMI
  Strings::ReplaceAllIn(result, "IEC958", "Digital/HDMI");

  return Strings::Trim(result);
}

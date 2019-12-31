//
// Created by bkg2k on 28/12/2019.
//

#include "BiosList.h"
#include <utils/Log.h>

BiosList::BiosList(XmlNode& systemNode)
{
  // Extract system
  mSystemFullName = systemNode.attribute("fullname").value();
  mSystemName = systemNode.attribute("platform").value();
  if (mSystemFullName.empty() || mSystemName.empty())
    LOG(LogWarning) << "Bios file's system node is missing fullname and/or platform attributes!";

  // Extract bios
  for(XmlNode& biosNode : systemNode.children("bios"))
  {
    Bios bios(biosNode);
    if (bios.IsValid())
      mBiosList.push_back(bios);
  }
}

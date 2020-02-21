//
// Created by bkg2k on 28/12/2019.
//

#include <RootFolders.h>
#include <utils/Log.h>
#include <algorithm>
#include "BiosManager.h"

BiosManager::BiosManager()
  : StaticLifeCycleControler<BiosManager>("BiosManager"),
    mSender(this),
    mReporting(nullptr)
{
}

void BiosManager::LoadFromFile()
{
  Path xmlpath = RootFolders::TemplateRootFolder / sBiosFilePath;

  XmlDocument biosListXml;
  XmlResult result = biosListXml.load_file(xmlpath.ToChars());
  if (!result)
  {
    LOG(LogError) << "Could not parse " << xmlpath.ToString() << " file!";
    return;
  }

  XmlNode list = biosListXml.child("biosList");
  for(XmlNode& systemNode : list.children("system"))
  {
    BiosList biosList(systemNode);
    mSystemBiosList.push_back(biosList);
  }

  std::sort(mSystemBiosList.begin(), mSystemBiosList.end(), [](const BiosList& a, const BiosList& b) { return a.FullName() < b.FullName(); });
}

void BiosManager::Scan(IBiosScanReporting* reporting)
{
  if (!IsRunning())
  {
    mReporting = reporting;
    Thread::Start("Bios-Scan");
  }
}

void BiosManager::Run()
{
  // Scan
  for(int l = 0; l < SystemCount(); ++l)
  {
    BiosList& biosList = mSystemBiosList[l];
    for(int i = 0; i < biosList.BiosCount(); ++i)
    {
      biosList.ScanAt(i);
      mSender.Call((l << 16) + i, mReporting);
    }
  }

  // End of scan
  mSender.Call(-1, mReporting);

  // Nullify interface
  mReporting = nullptr;
}

void BiosManager::ReceiveSyncCallback(const SDL_Event& event)
{
  // Extract interface
  IBiosScanReporting* reporting = ((IBiosScanReporting*)event.user.data1);

  if (event.user.code < 0)
  {
    if (reporting != nullptr)
      reporting->ScanComplete();
    // Allow the thread to be restarted
    Thread::Stop();
  }
  else
  {
    // Extraxt data
    int biosIndex = event.user.code & 0xFFFF;
    int biosListIndex = (event.user.code >> 16) & 0xFFFF;

    // Call interface
    if (reporting != nullptr)
      reporting->ScanProgress(SystemBios(biosListIndex).BiosAt(biosIndex));
  }
}

const BiosList* BiosManager::SystemBios(const std::string& name)
{
  for(const BiosList& biosList : mSystemBiosList)
    if (biosList.Name() == name)
      return &biosList;

  return nullptr;
}


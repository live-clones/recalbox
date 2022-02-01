//
// Created by bkg2k on 28/12/2019.
//

#include <RootFolders.h>
#include <utils/Log.h>
#include <utils/Files.h>
#include <utils/datetime/DateTime.h>
#include "BiosManager.h"

BiosManager::BiosManager()
  : StaticLifeCycleControler<BiosManager>("BiosManager")
  #ifndef PURE_BIOS_ONLY
  , mSender(*this)
  #endif
  , mReporting(nullptr)
{
}

void BiosManager::LoadFromFile()
{
  Path xmlpath = RootFolders::TemplateRootFolder / sBiosFilePath;

  XmlDocument biosListXml;
  XmlResult result = biosListXml.load_file(xmlpath.ToChars());
  if (!result)
  {
    { LOG(LogError) << "[Bios] Could not parse " << xmlpath.ToString() << " file!"; }
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

void BiosManager::Scan(IBiosScanReporting* reporting, bool sync)
{
  if (sync)
  {
    mReporting = reporting;
    Run();
  }
  else if (!IsRunning())
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
      #ifndef PURE_BIOS_ONLY
        mSender.Send(BiosMessage::SingleBiosReport(mReporting, l, i));
      #endif
    }
  }

  // End of scan
  #ifndef PURE_BIOS_ONLY
    mSender.Send(BiosMessage::EndBiosReport(mReporting));
  #endif

  // Nullify interface
  mReporting = nullptr;

  // Generate report
  GenerateReport();
}

#ifndef PURE_BIOS_ONLY
void BiosManager::ReceiveSyncMessage(const BiosMessage& message)
{
  // Extract interface
  IBiosScanReporting* reporting = message.mReporting;

  if (message.mComplete)
  {
    if (reporting != nullptr)
      reporting->ScanComplete();
    // Allow the thread to be restarted
    Thread::Stop();
  }
  else
  {
    // Call interface
    if (reporting != nullptr)
      reporting->ScanProgress(SystemBios(message.mListIndex).BiosAt(message.mIndex));
  }
}
#endif

const BiosList& BiosManager::SystemBios(const std::string& name)
{
  for(const BiosList& biosList : mSystemBiosList)
    if (biosList.Name() == name)
      return biosList;

  static BiosList sEmptyBiosList;
  return sEmptyBiosList;
}

BiosManager::LookupResult BiosManager::Lookup(const std::string& name, const std::string& md5, const Bios*& outputBios)
{
  outputBios = nullptr;

  // MD5 first
  for(const BiosList& biosList : mSystemBiosList)
    for(int i = biosList.BiosCount(); --i >= 0; )
    {
      const Bios& bios = biosList.BiosAt(i);
      if (bios.IsMD5Known(md5))
      {
        outputBios = &bios;
        return (bios.BiosStatus() == Bios::Status::HashMatching) ? LookupResult::AlreadyExists : LookupResult::Found;
      }
    }

  // Name & no matching mandatory
  for(const BiosList& biosList : mSystemBiosList)
    for(int i = biosList.BiosCount(); --i >= 0; )
    {
      const Bios& bios = biosList.BiosAt(i);
      if (name == bios.Filepath().Filename() && !bios.IsHashMatchingMandatory())
      {
        outputBios = &bios;
        return LookupResult::Found;
      }
    }

  return LookupResult::NotFound;
}

void BiosManager::GenerateReport() const
{
  std::string report = "==============================================================\r\n"
                       "MISSING BIOS REPORT\r\n"
                       "Platform: #ARCH#\r\n"
                       "Generated on #DATE#\r\n"
                       "==============================================================\r\n\r\n";
  Strings::ReplaceAllIn(report, "#ARCH#", Files::LoadFile(Path("/recalbox/recalbox.arch")));
  Strings::ReplaceAllIn(report, "#DATE#", DateTime().ToLongFormat());
  for(const BiosList& biosList : mSystemBiosList)
  {
    std::string subReport = biosList.GenerateReport();
    if (!subReport.empty())
      report.append(subReport);
  }

  Files::SaveFile(RootFolders::DataRootFolder / sReportPath, report);
}

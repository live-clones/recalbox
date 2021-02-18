//
// Created by bkg2k on 28/12/2019.
//

#include <utils/Log.h>
#include <utils/hash/Md5.h>
#include <utils/Files.h>
#include <RootFolders.h>
#include <utils/Zip.h>
#include "Bios.h"

Bios::Md5Hash::Md5Hash(const std::string& source)
  : Md5Hash()
{
  if (source.length() != sizeof(mBytes) * 2)
  {
    { LOG(LogError) << "[Bios] Invalid MD5: " << source; }
    mValid = false;
    return;
  }

  // Deserialize
  for(int i = (int)sizeof(mBytes); --i >= 0;)
  {
    unsigned char b = 0;
    unsigned char cl = (source[i * 2 + 0]) | 0x20 /* force lowercase - do not disturb digits */;
    unsigned char ch = (source[i * 2 + 1]) | 0x20 /* force lowercase - do not disturb digits */;
    b |= ((cl - 0x30) <= 9 ? (cl - 0x30) : ((cl - 0x61) <= 5) ? (cl - 0x61 + 10) : 0) << 4;
    b |= ((ch - 0x30) <= 9 ? (ch - 0x30) : ((ch - 0x61) <= 5) ? (ch - 0x61 + 10) : 0) << 0;
    mBytes[i] = b;
  }
  mValid = true;
}

std::string Bios::Md5Hash::ToString() const
{
  char hashStr[sizeof(mBytes) * 2];

  // Serialize
  for(int i = (int)sizeof(mBytes); --i >= 0;)
  {
    hashStr[i * 2 + 0] = "0123456789ABCDEF"[mBytes[i] >> 4];
    hashStr[i * 2 + 1] = "0123456789ABCDEF"[mBytes[i] & 15];
  }

  return std::string(hashStr, sizeof(hashStr));
}

Bios::Bios()
  : mMandatory(false),
    mHashMatchMandatory(false),
    mStatus(Status::Unknown),
    mReportStatus(ReportStatus::Unknown)
{
}

void Bios::Scan()
{
  // Scan
  bool found = false;
  for(int i = sMaxBiosPath; --i >= 0;)
  {
    if (!mPath[i].Exists()) continue;
    if (Strings::ToLowerASCII(mPath[i].Extension()) == ".zip")
    {
      // Get composite hash from the zip file
      std::string md5string = Zip(mPath[i]).Md5Composite();
      mRealFileHash = Md5Hash(md5string);
    }
    else
    {
      // Load bios
      std::string biosContent = Files::LoadFile(mPath[i]);

      // Compute md5
      MD5 md5;
      md5.update(biosContent.data(), biosContent.length());
      md5.finalize();
      mRealFileHash = Md5Hash(md5);
    }
    found = true;
  }

  // Not found
  if (!found)
  {
    mStatus = Status::FileNotFound;
    mReportStatus = mMandatory ? ReportStatus::Red : ReportStatus::Yellow;
    return;
  }

  // Matching?
  bool matching = false;
  for(Md5Hash& m : mHashes)
    if ((matching = mRealFileHash.IsMatching(m)))
      break;

  // Get result
  if (matching)
  {
    mStatus = Status::HashMatching;
    mReportStatus = ReportStatus::Green;
  }
  else
  {
    mStatus = Status::HashNotMatching;
    mReportStatus = mHashMatchMandatory && mMandatory ? ReportStatus::Red : ReportStatus ::Yellow;
  }
}

bool Bios::IsForCore(const std::string& core) const
{
  // Contains?
  unsigned long long pos = mCores.find(core);
  if (pos == std::string::npos) return false;

  // Start?
  if (pos != 0)
    if (mCores[pos - 1] != ',')
      return false;

  // End?
  if (pos + core.length() < mCores.length())
    if (mCores[pos + core.length()] != ',')
      return false;

  return true;
}

Bios::Bios(const XmlNode& biosNode)
  : mStatus(Status::Unknown),
    mReportStatus(ReportStatus::Unknown)
{
  // Load mandatory fields
  pugi::xml_attribute path = biosNode.attribute("path");
  if (!path) { LOG(LogError) << "[Bios] Bios file's bios node is missing path!"; return; }
  pugi::xml_attribute hashes = biosNode.attribute("md5");
  if (!hashes) { LOG(LogError) << "[Bios] Bios file's bios node is missing md5!"; return; }
  pugi::xml_attribute cores = biosNode.attribute("core");
  if (!cores) { LOG(LogError) << "[Bios] Bios file's bios node is missing cores!"; return; }

  // Set mandatory fields
  Strings::Vector list = Strings::Split(path.value(), '|');
  for(int i = sMaxBiosPath; --i >= 0; )
    if ((int)list.size() > i)
    {
      mPath[i] = Path(list[i]);
      if (!mPath[i].IsAbsolute())
        mPath[i] = RootFolders::DataRootFolder / "bios" / path.value();
    }

  mCores = cores.value();
  Strings::Vector md5list = Strings::Split(hashes.value(), ',');
  for(std::string& md5string : md5list)
  {
    Md5Hash md5(Strings::Trim(md5string));
    if (md5.IsValid())
      mHashes.push_back(md5);
  }

  // Optionnal fields
  mMandatory = strcmp(biosNode.attribute("mandatory").value(), "false") != 0;
  mHashMatchMandatory = strcmp(biosNode.attribute("hashMatchMandatory").value(), "false") != 0;
  mNotes = biosNode.attribute("note").value();
}

std::vector<std::string> Bios::MD5List() const
{
  std::vector<std::string> result;
  for(const Md5Hash& hash : mHashes)
    if (hash.IsValid())
      result.push_back(hash.ToString());
  return result;
}

std::string Bios::Filename(bool shorten) const
{
  bool ok = false;

  // Try to make relative to the bios folder
  Path rootPath(RootFolders::DataRootFolder);
  std::string result = mPath[0].MakeRelative(rootPath, ok).ToString();
  // Too long?
  if (shorten)
    if (Strings::CountChar(result, '/') > 2)
      result = std::string(".../").append(mPath[0].Filename());

  return result;
}

std::string Bios::GenerateReport() const
{
  std::string report;

  switch(mStatus)
  {
    case Status::FileNotFound:
    case Status::HashNotMatching:
    {
      // Ignore No matching case when it's not required
      if (mStatus == Status::HashNotMatching && !mHashMatchMandatory) break;

      // BIOS
      report.append("  ").append(mStatus == Status::FileNotFound ? "MISSING " : "INCORRECT ")
            .append(mMandatory ? "REQUIRED " : "OPTIONAL ")
            .append("BIOS: ").append(mPath[0].Filename()).append("\r\n");

      // Information
      report.append("    Path: ").append(mPath[0].ToString());
      for(int i = sMaxBiosPath; --i >= 1; )
        if (!mPath[i].IsEmpty()) report.append(" or ").append(mPath[i].ToString());
      report.append("\r\n");
      if (!mNotes.empty()) report.append("    Notes: ").append(mNotes).append("\r\n");
      if (!mCores.empty()) report.append("    For: ").append(mCores).append("\r\n");

      // MD5
      if (mStatus == Status::HashNotMatching)
        report.append("    Current MD5: ").append(mRealFileHash.ToString()).append("\r\n");
      report.append("    Possible MD5 List:\r\n");
      for(const Md5Hash& hash : mHashes)
        report.append("      ").append(hash.ToString()).append("\r\n");
      break;
    }
    case Status::Unknown:
    case Status::HashMatching:
    default: break;
  }

  return report;
}


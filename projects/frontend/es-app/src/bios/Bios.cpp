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
    LOG(LogError) << "Invalid MD5: " << source;
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
  if (!mPath.Exists())
  {
    mStatus = Status::FileNotFound;
    mReportStatus = mMandatory ? ReportStatus::Red : ReportStatus::Yellow;
    return;
  }

  if (Strings::ToLowerASCII(mPath.Extension()) == ".zip")
  {
    // Get composite hash from the zip file
    std::string md5string = Zip(mPath).Md5Composite();
    mRealFileHash = Md5Hash(md5string);
  }
  else
  {
    // Load bios
    std::string biosContent = Files::LoadFile(mPath);

    // Compute md5
    MD5 md5;
    md5.update(biosContent.data(), biosContent.length());
    md5.finalize();
    mRealFileHash = Md5Hash(md5);
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
  if (!path) { LOG(LogError) << "Bios file's bios node is missing path!"; return; }
  pugi::xml_attribute hashes = biosNode.attribute("md5");
  if (!hashes) { LOG(LogError) << "Bios file's bios node is missing md5!"; return; }
  pugi::xml_attribute cores = biosNode.attribute("core");
  if (!cores) { LOG(LogError) << "Bios file's bios node is missing cores!"; return; }

  // Set mandatory fields
  mPath = Path(path.value());
  if (!mPath.IsAbsolute())
    mPath = RootFolders::DataRootFolder / "bios" / path.value();
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

std::string Bios::Filename() const
{
  bool ok;
  /*
  // Try to make relative to the bios folder
  Path rootPath(RootFolders::DataRootFolder / "bios");
  std::string result = mPath.MakeRelative(rootPath, ok).ToString();
  // Failed? Try to make relative to the rom folder
  if (!ok)
  {
    rootPath = RootFolders::DataRootFolder / "roms";
    result = mPath.MakeRelative(rootPath, ok).ToString();
  }

  // Too long?
  if (Strings::CountChar(result, '/') > 1)
    result = std::string(".../").append(mPath.Filename());
*/

  // Try to make relative to the bios folder
  Path rootPath(RootFolders::DataRootFolder);
  std::string result = mPath.MakeRelative(rootPath, ok).ToString();
  // Too long?
  if (Strings::CountChar(result, '/') > 2)
    result = std::string(".../").append(mPath.Filename());

  return result;
}


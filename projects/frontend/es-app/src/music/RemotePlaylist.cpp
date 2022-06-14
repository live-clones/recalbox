//
// Created by bkg2k on 29/05/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "RemotePlaylist.h"
#include "rapidjson/document.h"
#include "recalbox/RecalboxSystem.h"
#include <patreon/PatronInfo.h>
#include <RecalboxConf.h>
#include <utils/Files.h>

RemotePlaylist::RemotePlaylist()
  : StaticLifeCycleControler("RemotePlaylist")
  , mStorageRoot(sStorageRootPath)
  , mNextIndex(-1)
{
  Thread::Start("RemotePL");
}

RemotePlaylist::~RemotePlaylist()
{
  mRequest.Cancel();
  mAction.Fire();
  Thread::Stop();
}

void RemotePlaylist::Run()
{
  // Wait for Patreon response
  PatronInfo::Instance().WaitForAuthentication(*this);

  // Check patron status
  if (PatronInfo::Instance().Status() == PatronAuthenticationResult::Patron)
  {
    // Update playlist
    UpdatePlayList();
    // Load & check index
    InitializeIndex();
    // Let's go for action
    mAction.Fire();

    // Main loop
    while(IsRunning())
    {
      // Wait for action
      mAction.WaitSignal();
      if (!IsRunning()) break;
      // Sanitize files
      SanitizeFiles();
      // Download new files
      DownloadFiles();
    }
  }
  else { LOG(LogError) << "[RemotePlaylist] Not a patron"; }
}

bool RemotePlaylist::DeserializePlaylist(const std::string& jsonString)
{
  // Deserialize
  rapidjson::Document json;
  json.Parse(jsonString.c_str());
  if (!json.HasParseError() && json.IsArray())
  {
    for(const rapidjson::Value& object : json.GetArray())
    {
      if (object.HasMember("name") &&
          object.HasMember("time_seconds") &&
          object.HasMember("size") &&
          object.HasMember("url") &&
          object.HasMember("md5") &&
          object.HasMember("album") &&
          object.HasMember("mixtape"))
        mPlaylist.push_back(TrackInfo(object["name"].GetString(),
                                      object["time_seconds"].GetInt(),
                                      object["size"].GetInt(),
                                      object["url"].GetString(),
                                      object["md5"].GetString(),
                                      object["album"].GetString(),
                                      object["mixtape"].GetString()));
    }
    // Cache list
    Files::SaveFile(Path(sStorageList), jsonString);
    return true;
  }
  { LOG(LogError) << "[RemotePlaylist] Error getting playlist. Malformed json: " << jsonString; }
  return false;
}

bool RemotePlaylist::UpdatePlayList()
{
  std::string jsonString;

  // Remote
  mRequest.SetBearer(RecalboxConf::Instance().GetRecalboxPrivateKey());
  if (mRequest.Execute(sRemotePlayListUrl, jsonString))
  {
    if (mRequest.GetLastHttpResponseCode() == 200)
      return DeserializePlaylist(jsonString);
    { LOG(LogError) << "[RemotePlaylist] Error getting playlist. Error code: " << mRequest.GetLastHttpResponseCode(); }
    return false;
  }
  { LOG(LogError) << "[RemotePlaylist] Error getting playlist."; }

  // Local
  jsonString = Files::LoadFile(Path(sStorageList));
  return DeserializePlaylist(jsonString);
}

void RemotePlaylist::InitializeIndex()
{
  // Initialize main path
  mStorageRoot.CreatePath();

  // Try loading index
  Path indexPath(sStorageIndex);
  std::string md5Index = Files::LoadFile(indexPath);
  mNextIndex = LookupTrackIndex(md5Index);
  if (mNextIndex < 0) mNextIndex = 0;
}

void RemotePlaylist::UpdateIndex()
{
  if ((unsigned int)mNextIndex >= (unsigned int)mPlaylist.size()) return;
  Path indexPath(sStorageIndex);
  Files::SaveFile(indexPath, mPlaylist[mNextIndex].Md5());
}

int RemotePlaylist::LookupTrackIndex(const std::string& md5)
{
  for(int i = (int)mPlaylist.size(); --i >= 0; )
    if (md5 == mPlaylist[i].Md5())
      return i;
  return -1;
}

void RemotePlaylist::SanitizeFiles()
{
  if (mPlaylist.empty()) return;

  // Remove unknown files
  for(const Path& file : Path(sStorageRootPath).GetDirectoryContent())
    if (LookupTrackIndex(file.FilenameWithoutExtension()) < 0)
    {
      file.Delete();
      { LOG(LogError) << "[RemotePlaylist] File " << file.ToString() << " not in playlist. Deleted."; }
    }

  // Remove files out index/index+X range
  // Only if network is available
  if (RecalboxSystem::hasIpAdress(false))
  {
    mLocker.Lock();
    int first = mNextIndex;
    int last = (mNextIndex + sMaxCachedFile - 1) % (int) mPlaylist.size();
    mLocker.UnLock();
    for (int i = (int) mPlaylist.size(); --i >= 0 && IsRunning();)
    {
      const Path& file = mPlaylist[i].LocalPath();
      if (file.Exists())
        if (((first < last) && (i < first || i >= last)) || ((first > last) && (i > last && i < first)) ||
            !mPlaylist[i].HasValidFile())
        {
          file.Delete();
          { LOG(LogError) << "[RemotePlaylist] File " << file.ToString() << " out of index range. Deleted."; }
        }
    }
  }
}

void RemotePlaylist::DownloadFiles()
{
  if (mPlaylist.empty()) return;

  // Loop until there is nothing more to download
  for(bool download = true; download && IsRunning();)
  {
    download = false;
    mLocker.Lock();
    int index = mNextIndex;
    mLocker.UnLock();
    for(int i = sMaxCachedFile; --i >= 0 && IsRunning(); )
    {
      TrackInfo& track = mPlaylist[index];
      if (!track.LocalPath().Exists())
      {
        { LOG(LogDebug) << "[RemotePlaylist] Downloading " << track.LocalPath().ToString(); }
        if (!mRequest.Execute(track.Url(), track.LocalPath()))
        {
          { LOG(LogError) << "[RemotePlaylist] Failed to download " << track.Url() << ". Retry in a few seconds."; }
          Thread::Sleep(5000);
          continue;
        }
        if (mRequest.GetLastHttpResponseCode() != 200)
        {
          { LOG(LogError) << "[RemotePlaylist] Failed to download " << track.Url() << ". Http Result code: " << mRequest.GetLastHttpResponseCode() << ". Retry in a few seconds."; }
          Thread::Sleep(5000);
          continue;
        }
        if (Strings::ToLowerASCII(mRequest.Md5()) != Strings::ToLowerASCII(track.Md5()))
        {
          { LOG(LogError) << "[RemotePlaylist] Failed to download " << track.Url() << ". MD5 mismatch. Retry."; }
          continue;
        }
        { LOG(LogDebug) << "[RemotePlaylist] Downloaded " << track.LocalPath().ToString(); }
        download = true;
      }
      index = (index + 1) % (int)mPlaylist.size();
    }
  }
}

RemotePlaylist::TrackInfo* RemotePlaylist::GetNextTrack()
{
  if (mPlaylist.empty()) return nullptr;
  if (!RecalboxConf::Instance().GetMusicRemoteEnable()) return nullptr;

  {
    Mutex::AutoLock locker(mLocker);
    for(int i = sMaxCachedFile; --i >= 0 && IsRunning(); mNextIndex = (mNextIndex + 1) % (int)mPlaylist.size())
      if (mPlaylist[mNextIndex].HasValidFile())
      {
        UpdateIndex();
        return &mPlaylist[mNextIndex];
      }
  }
  // No file found, let's go download
  mAction.Fire();
  return nullptr;
}

void RemotePlaylist::TrackConsumed()
{
  Mutex::AutoLock locker(mLocker);
  mNextIndex = (mNextIndex + 1) % (int)mPlaylist.size();
  UpdateIndex();
  mAction.Fire();
}


//
// Created by bkg2k on 05/12/2019.
//

#include "Http.h"
#include "utils/Files.h"
#include "utils/Log.h"

Http::DownloadInfo Http::sDownloadStorage[sMaxBandwidthInfo];
int Http::sDownloadCount = 0;
int Http::sDownloadIndex = 0;
Mutex Http::sDownloadLocker;

Http::Http() noexcept
  : mHandle(curl_easy_init())
  , mIDownload(nullptr)
  , mContentSize(0)
  , mContentLength(0)
  , mLastReturnCode(0)
  , mCancel(false)
{
  if (mHandle != nullptr)
  {
    curl_easy_setopt(mHandle, CURLOPT_REDIR_PROTOCOLS,CURLPROTO_HTTP | CURLPROTO_HTTPS);
    curl_easy_setopt(mHandle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(mHandle, CURLOPT_TCP_KEEPIDLE, 240L);
    curl_easy_setopt(mHandle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(mHandle, CURLOPT_WRITEDATA, this);
  }
}

Http::~Http()
{
  if (mHandle != nullptr)
    curl_easy_cleanup(mHandle);
}

bool Http::Execute(const std::string& url, std::string& output)
{
  if (mHandle != nullptr)
  {
    DateTime start;
    mContentSize = 0;
    mLastReturnCode = 0;
    mResultHolder.clear();
    mResultFile = Path::Empty;
    DataStart();
    curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(mHandle);
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mLastReturnCode);
    StoreDownloadInfo(start, DateTime(), mContentSize);
    output = std::move(mResultHolder);
    bool ok = (res == CURLcode::CURLE_OK);
    if (ok) DataEnd();
    return ok;
  }
  return false;
}

bool Http::Execute(const std::string& url, const Path& output)
{
  if (mHandle != nullptr)
  {
    if (!output.Directory().Exists())
      output.Directory().CreatePath();
    DateTime start;
    mContentSize = 0;
    mLastReturnCode = 0;
    mResultHolder.clear();
    mResultFile = output;
    mResultFile.Delete();
    DataStart();
    curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(mHandle);
    WriteCallback(nullptr, 0, 0, this);
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mLastReturnCode);
    StoreDownloadInfo(start, DateTime(), mContentSize);
    bool ok = (res == CURLcode::CURLE_OK);
    if (!ok) output.Delete();
    else DataEnd();
    return ok;
  }
  return false;
}

bool Http::Execute(const std::string& url, const Path& output, Http::IDownload* interface)
{
  mIDownload = interface;
  return Execute(url, output);
}

size_t Http::WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
  Http& This = *((Http*)userdata);

  return This.DoDataReceived(ptr, (int)(size * nmemb));
}

size_t Http::DoDataReceived(const char* data, int length)
{
  // Always store into the string
  mResultHolder.append(data, length);
  mContentSize += (long long)(length);

  // Get content length
  if (mContentLength == 0)
  {
    double contentLength = 0;
    curl_easy_getinfo(mHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLength);
    mContentLength = (long long)contentLength;
  }

  // Callback?
  if (mIDownload != nullptr)
    mIDownload->DownloadProgress(*this, mContentSize, mContentLength);

  // Inherited procesing
  DataReceived(data, length);

  // Should flush?
  if (!mResultFile.IsEmpty())
    if (mResultHolder.length() > sMaxDataKeptInRam || length == 0)
    {
      // Try flushing to disk
      if (!Files::AppendToFile(mResultFile, mResultHolder.c_str(), (int)mResultHolder.length()))
        return 0; // Error flushing to disk
      // Clear the string
      mResultHolder.clear();
    }

  return mCancel ? 0 : length;
}

void Http::Cancel()
{
  mCancel = true;
}

void Http::SetBasicAuth(const std::string& login, const std::string& password)
{
  if (mHandle != nullptr)
  {
    // Hold strings
    mLogin = login;
    mPassword = password;
    // Set basic auth
    curl_easy_setopt(mHandle, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
    curl_easy_setopt(mHandle, CURLOPT_USERNAME, mLogin.data());
    curl_easy_setopt(mHandle, CURLOPT_PASSWORD, mPassword.data());
  }
}

void Http::SetBearer(const std::string& bearer)
{
  if (mHandle != nullptr)
  {
    // Hold strings
    mBearer = bearer;
    // Set bearer auth
    struct curl_slist *list = nullptr;
    const std::string header = std::string("Authorization: Bearer ")+bearer;
    list = curl_slist_append(list, header.c_str());

    curl_easy_setopt(mHandle, CURLOPT_HTTPHEADER, list);
  }
}

void Http::CancelBasicAuth()
{
  if (mHandle != nullptr)
  {
    // Reset strings
    mLogin.clear();
    mPassword.clear();
    // Set basic auth
    curl_easy_setopt(mHandle, CURLOPT_HTTPAUTH, (long)CURLAUTH_NONE);
    curl_easy_setopt(mHandle, CURLOPT_USERNAME, mLogin.data());
    curl_easy_setopt(mHandle, CURLOPT_PASSWORD, mPassword.data());
  }
}

void Http::StoreDownloadInfo(const DateTime& start, const DateTime& stop, long long size)
{
  long long elapsed = (stop - start).TotalMilliseconds();
  Mutex::AutoLock locker(sDownloadLocker);
  DownloadInfo& info = sDownloadStorage[sDownloadIndex];
  info.Time = elapsed;
  info.Size = size;
  if (++sDownloadIndex >= sMaxBandwidthInfo) sDownloadIndex = 0;
  sDownloadCount++;

  LOG(LogDebug) << "[Http] Average Bandwidth: " << (float)GetAverageBandwidth() << " bytes/s.";
}

double Http::GetAverageBandwidth()
{
  if (sDownloadCount < 16) return -1; // Not enough data
  Mutex::AutoLock locker(sDownloadLocker);
  int count = sDownloadCount > sMaxBandwidthInfo ? sMaxBandwidthInfo : sDownloadCount;
  long long accumulatedBytes = 0;
  long long accumulatedTime = 0;
  for(int i = count; --i >= 0;)
  {
    accumulatedBytes += sDownloadStorage[i].Size;
    accumulatedTime += sDownloadStorage[i].Time;
  }
  return (double)accumulatedBytes / ((double)accumulatedTime / 1000.0);
}

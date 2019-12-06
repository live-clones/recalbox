//
// Created by bkg2k on 05/12/2019.
//

#include "Http.h"
#include "Files.h"

Http::Http() noexcept
  : mHandle(nullptr)
{
  mHandle = curl_easy_init();
  if (mHandle != nullptr)
  {
    curl_easy_setopt(mHandle, CURLOPT_REDIR_PROTOCOLS,CURLPROTO_HTTP | CURLPROTO_HTTPS);
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
    mLastReturnCode = 0;
    mResultHolder.clear();
    mResultFile = Path::Empty;
    curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(mHandle);
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mLastReturnCode);
    output = std::move(mResultHolder);
    return res == CURLcode::CURLE_OK;
  }
  return false;
}

bool Http::Execute(const std::string& url, const Path& output)
{
  if (mHandle != nullptr)
  {
    mLastReturnCode = 0;
    mResultHolder.clear();
    mResultFile = output;
    mResultFile.Delete();
    curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(mHandle);
    WriteCallback(nullptr, 0, 0, this);
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mLastReturnCode);
    return res == CURLcode::CURLE_OK;
  }
  return false;
}

size_t Http::WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
  Http& This = *((Http*)userdata);
  // Always store into the string
  This.mResultHolder.append(ptr, size * nmemb);
  // Should flush?
  if (!This.mResultFile.IsEmpty() && (This.mResultHolder.length() > sMaxDataKeptInRam || (size * nmemb) == 0))
  {
    // Try flushing to disk
    if (!Files::AppendToFile(This.mResultFile, This.mResultHolder.c_str(), (int)This.mResultHolder.length()))
      return 0; // Error flushing to disk
    // Clear the string
    This.mResultHolder.clear();
  }
  return size * nmemb;
}

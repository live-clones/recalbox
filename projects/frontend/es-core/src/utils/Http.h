//
// Created by bkg2k on 05/12/2019.
//
#pragma once

#include <string>
#include <utils/os/fs/Path.h>
#include "curl/curl.h"

class Http
{
  private:
    //! Maximum data kept in memory before flushing to file
    static constexpr int sMaxDataKeptInRam = (2 << 20); // 20Mb

    //! CURL Handle for all requests. Allocated once
    CURL* mHandle;
    //! HTTP returnc ode of last request
    int   mLastReturnCode;

    //! Request result, or temporary buffer waiting for disk flush
    std::string mResultHolder;
    //! Store request result into this file if it's not empty
    Path        mResultFile;

    /*!
     * @brief CURL callback when recveiving data
     * @param ptr Data pointer
     * @param size Data size multiplier (always 1)
     * @param nmemb Data size (real)
     * @param userdata Userdata pointer (pointer to class instance)
     * @return Must return nmemb when fully processed. Any other value means an error
     */
    static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata);

  public:

    /*!
     * @brief Default constructor
     */
    Http() noexcept;

    /*!
     * @brief Destructor
     */
    ~Http();

    /*!
     * @brief Execute an HTTP Request ang get the result into a string
     * @param url Target URL
     * @param output String to write request result into
     * @return True if the request executed successfuly
     */
    bool Execute(const std::string& url, std::string& output);

    /*!
     * @brief Execute an HTTP Request ang save the result into a file
     * @param url Target URL
     * @param output File path to save request result into
     * @return True if the request executed successfuly
     */
    bool Execute(const std::string& url, const Path& output);
};


/*size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  std::string s(ptr, size * nmemb);
  puts(s.c_str());
}

int main()
{
  CURL *curl = curl_easy_init();
  if(curl != nullptr)
  {
    curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS,CURLPROTO_HTTP | CURLPROTO_HTTPS);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, "http://google.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl);
    CURLcode res = curl_easy_perform(curl);
    int responseCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_cleanup(curl);
  }

  return 0;
}*/

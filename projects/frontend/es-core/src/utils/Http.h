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

    /*!
     * @brief Get last http response code
     * @return Http code, or 0 if the request failed to execute
     */
    int GetLastHttpResponseCode() const { return mLastReturnCode; }
};

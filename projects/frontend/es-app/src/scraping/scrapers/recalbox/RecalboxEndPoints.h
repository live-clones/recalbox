//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/screenscraper/IEndPointProvider.h>
#include <utils/network/DnsClient.h>
#include <utils/os/system/Mutex.h>

class RecalboxEndPoints : public IEndPointProvider
{
  public:
    //! Constructor
    RecalboxEndPoints();

    /*!
     * @brief Try to get durect user instead of URL for the request
     * @return Direct user object or nullptr
     */
    const ScreenScraperUser* GetDirectUserObject() const override;

    /*!
     * @brief Get base url for relative media
     * @return Base url
     */
    std::string GetUrlBase() override;

    /*!
     * @brief Scraper call this when an unexpected error occured in HTTP requests (timeount, 500, ...)
     */
    void NotifyError() override;

    /*!
     * @brief Provide a user info Url built from the given credentials
     * @param login Login
     * @param password Password
     * @return User info Url
     */
    std::string GetUserInfoUrl(const std::string& login, const std::string& password) override;

    /*!
     * @brief Provide a game info Url built from the given parameters
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param crc32 Rom CRC32
     * @param md5 Rom MD5
     * @param size Rom file size
     * @return Game info Url
     */
    std::string GetGameInfoUrl(const std::string& login, const std::string& password, const FileData& game,
                               const std::string& crc32, const std::string& md5, long long int size) override
    {
      (void)login;
      (void)password;
      (void)game;
      (void)crc32;
      (void)md5;
      (void)size;
      return std::string();
    }

    /*!
     * @brief Provide a game info Url built from the given parameters
     * Call it only if RequireSeparateRequests is true and a valid MD5 is provided
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param crc32 Rom CRC32
     * @param md5 Rom MD5
     * @param size Rom file size
     * @return Game info Url
     */
    std::string GetGameInfoUrlByMD5(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) override;

    /*!
     * @brief Provide a game info Url built from the given parameters
     * Call it only if RequireSeparateRequests is true and no MD5 is provided
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param size Rom file size
     * @return Game info Url
     */
    std::string GetGameInfoUrlByName(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) override;

    /*!
     * @brief Add (or not) query parameters to media request
     * @param url Url to decorate
     */
    void AddQueryParametersToMediaRequest(const FileData* game, long long size, std::string& url) override;

    /*!
     * @brief Recalbox do not use Basic Auth
     * @return False
     */
    bool RequireBasicAuth() override { return false; }

    /*!
     * @brief Recalbox uses Bearer
     * @return True
     */
    bool RequireBearer() override { return true; }

    /*!
     * @brief Tell the scraper, it needs to use separate requests whether MD5 hashes are available or not
     * @return
     */
    bool RequireSeparateRequests() override { return true; }

  private:
    //! Root domain name
    static constexpr const char* sRootDomainName = "https://scraper-rocketeer.recalbox.com";

    //! UUID
    std::string mUUID;
    //! Board
    std::string mBoard;
    //! Version
    std::string mVersion;

    //! Dns client
    DnsClient mDns;
    //! Mutex to protect error notification
    Mutex mErrorLocker;

    //! Target servers
    Strings::Vector mServers;
    //! Server index
    int mServerIndex;

    //! Total errors
    int mErrors;

    /*!
     * @brief Build standard query string
     * @param game Game to fetch data from
     * @return Query string
     */
    std::string BuildQueryString(const FileData* game, long long size);
};




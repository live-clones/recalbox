//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <games/FileData.h>
#include <scraping/scrapers/screenscraper/ScreenScraperUser.h>

class IEndPointProvider
{
  public:
    virtual ~IEndPointProvider() = default;

    /*!
     * @brief Check if the End point provider requires basic auth instead of custom auth (in the URL)
     * @return True if the implementation requires Basic Auth instead of URL
     */
    virtual bool RequireBasicAuth() = 0;

    /*!
     * @brief Check if the End point provider requires authorization bearer
     * @return True if the implementation requires Bearer Auth
     */
    virtual bool RequireBearer() = 0;

    /*!
     * @brief Tell the scraper, it needs to use separate requests whether MD5 hashes are available or not
     * @return
     */
    virtual bool RequireSeparateRequests() = 0;

    /*!
     * @brief Try to get durect user instead of URL for the request
     * @return Direct user object or nullptr
     */
    virtual const ScreenScraperUser* GetDirectUserObject() const = 0;

    /*!
     * @brief Scraper call this when an unexpected error occured in HTTP requests (timeount, 500, ...)
     */
    virtual void NotifyError() = 0;

    /*!
     * @brief Get base url for relative media
     * @return Base url
     */
    virtual std::string GetUrlBase() = 0;

    /*!
     * @brief Provide a user info Url built from the given credentials
     * @param login Login
     * @param password Password
     * @return User info Url
     */
    virtual std::string GetUserInfoUrl(const std::string& login, const std::string& password) = 0;

    /*!
     * @brief Provide a game info Url built from the given parameters
     * Call it only if RequireSeparateRequests is false
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param crc32 Rom CRC32
     * @param md5 Rom MD5
     * @param size Rom file size
     * @return Game info Url
     */
    virtual std::string GetGameInfoUrl(const std::string& login, const std::string& password, const FileData& game, const std::string& crc32, const std::string& md5, long long size) = 0;

    /*!
     * @brief Provide a game info Url built from the given parameters
     * Call it only if RequireSeparateRequests is true and a valid MD5 is provided
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param size Rom file size
     * @return Game info Url
     */
    virtual std::string GetGameInfoUrlByMD5(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) = 0;

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
    virtual std::string GetGameInfoUrlByName(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) = 0;

    /*!
     * @brief Add (or not) query parameters to media request
     * @param game game whose media are fetched
     * @param size game file size
     * @param url Url to decorate
     */
    virtual void AddQueryParametersToMediaRequest(const FileData* game, long long size, std::string& url) = 0;

    /*!
     * @brief Return the endpoint provider web url
     * @return Web url
     */
    virtual std::string GetProviderWebURL() = 0;
};
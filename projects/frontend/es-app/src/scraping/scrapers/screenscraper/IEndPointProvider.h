//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <utils/os/fs/Path.h>
#include <games/FileData.h>
#include <scraping/scrapers/screenscraper/ScreenScraperUser.h>

class IEndPointProvider
{
  public:
    virtual ~IEndPointProvider() = default;

    /*!
     * @brief Try to get durect user instead of URL for the request
     * @return Direct user object or nullptr
     */
    virtual const ScreenScraperUser* GetDirectUserObject() const = 0;

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
     * Call it only if GetGameInfoUrl failed - only related to filename, no hash at all!
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param size Rom file size
     * @return Game info Url
     */
    virtual std::string GetGameInfoUrl2(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) = 0;

    /*!
     * @brief Add (or not) query parameters to media request
     * @param game game whose media are fetched
     * @param url Url to decorate
     */
    virtual void AddQueryParametersToMediaRequest(const FileData* game, std::string& url) = 0;

    /*!
     * @brief Check if the End point provider requires basic auth instead of custom auth (in the URL)
     * @return True if the implementation requires Basic Auth instead of URL
     */
    virtual bool RequireBasicAuth() = 0;
};
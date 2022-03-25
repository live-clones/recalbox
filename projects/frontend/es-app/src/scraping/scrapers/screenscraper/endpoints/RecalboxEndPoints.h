//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/screenscraper/IEndPointProvider.h>

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
    std::string GetUrlBase() override { return "https://scraper-rocketeer.recalbox.com"; }

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
                               const std::string& crc32, const std::string& md5, long long int size) override;

    /*!
     * @brief Provide a game info Url built from the given parameters
     * Call it only if GetGameInfoUrl failed
     * @param login Login
     * @param password Password
     * @param system System ID
     * @param path Rom path
     * @param crc32 Rom CRC32
     * @param md5 Rom MD5
     * @param size Rom file size
     * @return Game info Url
     */
    std::string GetGameInfoUrl2(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) override;

    /*!
     * @brief Add (or not) query parameters to media request
     * @param url Url to decorate
     */
    void AddQueryParametersToMediaRequest(const FileData* game, std::string& url) override;

    /*!
     * @brief Recalbox uses Basic Auth
     * @return True
     */
    bool RequireBasicAuth() override { return true; }

  private:
    //! UUID
    std::string mUUID;
    //! Board
    std::string mBoard;
    //! Version
    std::string mVersion;

    /*!
     * @brief Build standard query string
     * @param game Game to fetch data from
     * @return Query string
     */
    std::string BuildQueryString(const FileData* game);
};




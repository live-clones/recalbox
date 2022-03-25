//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/screenscraper/IEndPointProvider.h>
#include "utils/os/fs/Path.h"

class ScreenScraperEndPoints : public IEndPointProvider
{
  public:
    /*!
     * @brief Try to get durect user instead of URL for the request
     * @return Direct user object or nullptr
     */
    const ScreenScraperUser* GetDirectUserObject() const override { return nullptr; }

    /*!
     * @brief Get base url for relative media
     * @return Base url
     */
    std::string GetUrlBase() override { return std::string(); }

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
    std::string GetGameInfoUrl(const std::string& login, const std::string& password, const FileData& game, const std::string& crc32, const std::string& md5, long long size) override;

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
    std::string GetGameInfoUrl2(const std::string& login, const std::string& password, const FileData& game, const std::string& md5, long long size) override
    {
      (void)login;
      (void)password;
      (void)game;
      (void)md5;
      (void)size;
      return std::string();
    }

    /*!
     * @brief Add (or not) query parameters to media request
     * @param url Url to decorate
     */
    void AddQueryParametersToMediaRequest(const FileData* game, std::string& url) override { (void)game; (void)url; }

    /*!
     * @brief ScreenScraper uses custom auth in URL parameters
     * @return False
     */
    bool RequireBasicAuth() override { return false; }

  private:
    static constexpr const char* API_DEV_U = "\xF1\x5A\xA8\x46\x25\xDE\x48\x2A";
    static constexpr const char* API_DEV_P = "\xC0\x0C\x80\x45\x30\xD6\x7F\x6A\x69\xB5\x02\x9D\xAD\x6B\xA3\x33\xE6\x7A\xE8\x4E";
    static constexpr const char* API_DEV_K = "\x83\x2E\xA9\xF4\x05\x67\xC1\xDB\xB1\x65\xC7\x0D\xFE\x29\xA3\x48";

    //! Api type
    enum class Api
    {
      UserInfo, //!< Get user information
      GameInfo, //!< Get game information
    };

    /*!
     * @brief Build the common URL part of all apis
     * @param api Api type
     * @return Url
     */
    static std::string BuildUrlCommon(Api api, const std::string& login, const std::string& password);

    //! X-Or the Space Sheriff
    static std::string XOrTheSpaceSheriff(const std::string& _input, const std::string& key);
};




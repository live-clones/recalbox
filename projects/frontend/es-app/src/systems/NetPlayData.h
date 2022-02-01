//
// Created by Bkg2k on 19/02/2020.
//
#pragma once

#include <string>

struct NetPlayData
{
  public:
    enum class Mode
    {
      None,
      Client,
      Server,
    };

  private:
    //! Shared netplay core
    std::string mCoreName;
    //! Target IP in client mode
    std::string mIP;
    //! Player password
    std::string mPlayerPassword;
    //! Viewer password
    std::string mViewerPassword;
    //! Target port in client mode or local port in server mode
    int mPort;
    //! True = server, false = client
    Mode mMode;
    //! Join as viewer in client mode
    bool mAsViewer;

  public:
    /*!
     * @brief Build a client Netplay data
     * @param coreName Libretro core
     * @param ip Target ip
     * @param port Target port
     */
    NetPlayData(const std::string& coreName, const std::string& ip, int port, const std::string& playerPassword, const std::string& viewerPassword, bool asViewer)
      : mCoreName(coreName),
        mIP(ip),
        mPlayerPassword(playerPassword),
        mViewerPassword(viewerPassword),
        mPort(port),
        mMode(Mode::Client),
        mAsViewer(asViewer)
    {
    }

    /*!
     * @brief Build a host netplay client
     */
    explicit NetPlayData(int port, const std::string& playerPassword, const std::string& viewerPassword)
      : mCoreName(),
        mIP(),
        mPlayerPassword(playerPassword),
        mViewerPassword(viewerPassword),
        mPort(port),
        mMode(Mode::Server),
        mAsViewer(false)
    {
    }

    /*!
     * @brief Build a no-netplay instance
     */
    NetPlayData()
      : mCoreName(),
        mIP(),
        mPlayerPassword(),
        mViewerPassword(),
        mPort(0),
        mMode(Mode::None),
        mAsViewer(false)
    {
    }

    /*
     * Accessors
     */

    //! Get core
    const std::string& CoreName() const { return mCoreName; }

    //! Get IP
    const std::string& Ip() const { return mIP; }

    //! Player password
    const std::string& PlayerPassword() const { return mPlayerPassword; }

    //! Viewer password
    const std::string& ViewerPassword() const { return mViewerPassword; }

    //! Get target/local port
    int Port() const { return mPort; }

    //! Check if this is a server mode
    Mode NetplayMode() const { return mMode; }

    //! Viewer only?
    bool IsViewerOnly() const { return mAsViewer; }
};
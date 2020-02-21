//
// Created by Bkg2k on 19/02/2020.
//
#pragma once

#include <string>

struct NetPlayData
{
  private:
    //! Shared netplay core
    std::string mCoreName;
    //! Target IP in client mode
    std::string mIP;
    //! Target port in client mode or local port in server mode
    int mPort;
    //! True = server, false = client
    bool mMode;

  public:
    /*!
     * @brief Build a client Netplay data
     * @param coreName Libretro core
     * @param ip Target ip
     * @param port Target port
     */
    NetPlayData(const std::string& coreName, const std::string& ip, int port)
      : mCoreName(coreName),
        mIP(ip),
        mPort(port),
        mMode(false)
    {
    }

    /*!
     * @brief Build a host netplay client
     */
    explicit NetPlayData(int port)
      : mCoreName(),
        mIP(),
        mPort(port),
        mMode(true)
    {
    }

    /*
     * Accessors
     */

    //! Get core
    const std::string& CoreName() const { return mCoreName; }

    //! Get IP
    const std::string& Ip() const { return mIP; }

    //! Get target/local port
    int Port() const { return mPort; }

    //! Check if this is a server mode
    bool IsServer() const { return mMode; }
};
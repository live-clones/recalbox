//
// Created by bkg2k on 30/03/2020.
//
#pragma once

#include <string>

class Parameters
{
  private:
    //! WWW Root. Default /recalbox/web
    std::string mWWWRoot;
    //! Root default file
    std::string mDefaultFile;
    //! Bind tp IP. Default is 0.0.0.0
    std::string mIP;
    //! Bind to port. Default is 20666
    int mPort;
    //! Number of simultaneous processor (threads)
    int mThreads;
    //! Debug log
    bool mDebug;

    /*!
     * @brief Display help message
     */
    static void Help();

  public:
    /*!
     * @brief Default constructor
     */
    Parameters()
      : mWWWRoot("/recalbox/web/manager-v3"),
        mDefaultFile("/index.html"),
        mIP("0.0.0.0"),
        mPort(20666),
        mThreads(5),
        mDebug(false)
    {
    }

    /*!
     * @brief Build a parameter class using command line arguments if available
     * @param argc Main argument count
     * @param argv Main arguments
     */
    Parameters(int argc, char* argv[]);

    /*!
     * @brief Log configuration
     */
    void LogConfig();

    /*
     * Getters
     */

    const std::string& WWWRoot() const { return mWWWRoot; }
    const std::string& DefaultFile() const { return mDefaultFile; }
    const std::string& IP() const { return mIP; }
    int Port() const { return mPort; }
    int Threads() const { return mThreads; }
    bool Debug() const { return mDebug; }
};

//
// Created by xizor on 06/06/18.
//
#pragma once

#include <WindowManager.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/os/system/Thread.h>

class NetPlayThread: private Thread, private ISynchronousEvent
{
  public:
    //! Typedef for convenience
    typedef std::vector<std::pair<std::string, std::string>> PlayerGameList;

    /*!
     * @brief Constructor
     * @param window main window
     */
    explicit NetPlayThread(WindowManager&window);

    /*!
     * @brief Destructor
     */
    ~NetPlayThread() override;

    /*!
     * Start the scan
     */
    void StartScan();
    /*!
     * Stop the scan
     */
    void StopScan();

    static std::string getLobbyListCommand();

    /*!
     * @brief Load current netplay list
     * @return Netplay list
     */
    static bool RefreshNetplayList(PlayerGameList& list, bool filtered);

  private:
    //! Attached window
    WindowManager& mWindow;

    //! SDL Event sender
    SyncronousEvent mSender;

    //! Last created popup
    std::string mLastPopupText;

    //! Main thread runner
    void Run() override;

    /*!
     * @brief Synchronous event receiver
     * @param event Event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*!
     * @brief Sleep a bit
     * @param enabled Netplay enable flag. May be updated.
     * @return True if the netplay flag has been updated
     */
    bool Sleep(bool& enabled);

    /*!
     * @brief Create and trig the popup
     * @param player Player name
     * @param game Game
     */
    void PopupTriggered(const std::string& player, const std::string& game);
};


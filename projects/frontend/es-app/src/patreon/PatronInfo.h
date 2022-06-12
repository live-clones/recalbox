//
// Created by digitalLumberjack on 08/05/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <utils/network/Http.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <utils/os/system/Thread.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <patreon/IPatreonNotification.h>

class PatronInfo : public StaticLifeCycleControler<PatronInfo>
                 , public Thread
                 , public ISynchronousEvent
{
  public:
    explicit PatronInfo(IPatreonNotification* callback);
    /*!
     * @brief Get the name of the patron
     * @returns the name of the patron, or empty string if not a patron
     * @note Multi thread access protected
     */
    std::string Name() { Mutex::AutoLock locker(mLocker); return mName; }

    /*!
     * @brief Get the patron state of the current user
     * @returns true if the user is a patron
     * @note Boolean are atomic values, there is no need to protect them against Multi thread access
     */
    bool IsPatron() const { return mResult == PatronAuthenticationResult::Patron; }

    /*!
     * @brief Get boss level (or 0 if the user is not a patron)
     * @return Boss level
     */
    int BossLevel() const { return mLevel; }

    /*!
     * @brief Get current patron status
     * @return Patron status
     */
    PatronAuthenticationResult Status() const { return mResult; }

    /*!
     * @brief Wait indefinitely for authentication to finish
     * Can take up to 4mn. Call from a thread only!
     * @param caller Calling thread
     */
    void WaitForAuthentication(Thread& caller);

  protected:
    /*!
     * @brief Non blocking initialization
     */
    void Run() override { Initialize(); mDone = true; }

  private:
    //! Timeout in milliseconds
    static constexpr const int sNetworkTimeout = 240 * 1000;
    //! Patreon api base url
    static constexpr const char *sRootDomainName = "https://api-patreon.recalbox.com";

    SyncronousEvent mEvent;             //!< Synchronous event to send/receive synchronized messages

    Mutex       mLocker;                //!< Prevent members from being accessed from different threads
    std::string mToken;                 //!< User toten from Patreon
    std::string mName;                  //!< Patreon name
    IPatreonNotification* mCallback;    //!< Callback interface
    PatronAuthenticationResult mResult; //!< Patron state & authentication result
    int mLevel;                         //!< Boss Level
    volatile bool mDone;                //!< Status of initial authentication request

    /*!
     * @brief Request patreon.com and update states accordingly
     */
    void Initialize();

    /*!
     * @brief Received synchronous events
     * @param event SCL2 event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;
};

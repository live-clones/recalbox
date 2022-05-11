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
    PatronInfo(IPatreonNotification* callback);
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
    bool IsPatron() const { return mIsPatron; }

  protected:
    /*!
     * @brief Non blocking initialization
     */
    void Run() override { Initialize(); }

  private:
    static constexpr const char *sRootDomainName = "https://api-patreon.recalbox.com";

    SyncronousEvent mEvent;          //!< Synchronous event to send/receive synchronized messages

    Mutex       mLocker;             //!< Prevent members from being accessed from different threads
    std::string mToken;              //!< User toten from Patreon
    std::string mName;               //!< Patreon name
    IPatreonNotification* mCallback; //!< Callback interface
    bool        mIsPatron;           //!< State whether the user is a patron or not

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

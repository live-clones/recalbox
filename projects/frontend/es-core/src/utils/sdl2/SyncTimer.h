#pragma once

#include <utils/os/system/Thread.h>
#include <utils/os/system/Signal.h>
#include "ISyncTimer.h"
#include <utils/sync/SyncMessageSender.h>
#include <utils/sync/ISyncMessageReceiver.h>

class SyncTimer : private Thread
                , private ISyncMessageReceiver<void>
{
  private:
    //! Synchronous Event manager
    SyncMessageSender<void> mSender;

    //! Timer's thread name
    std::string mName;

    //! Timer callbacl interface
    ISyncTimer* mCallback;

    //! User identifier
    int mIdentifier;

    //! Signla to unlock the thread
    Signal mSignal;

    //! Milliseconds before triggering the callback
    int mMilliseconds;

    //! True if the timer is repeating
    bool mRepeat;

    /*
     *  ISyncMessageReceiver implementation
     */

    /*!
     * @brief Synchronous event callback
     * @param event Received event
     */
    void ReceiveSyncMessage() override
    {
      if (mCallback != nullptr)
        mCallback->TimerTick(mIdentifier);
    }

    /*
     *  Thread implementation
     */

    /*!
     * @brief Time thread
     */
    void Run() override;

  public:
    /*!
     * @brief Default constructor
     * @param callback Timer callback
     * @param identifier Identifier passed to the timer callback to identify the source timer
     */
    SyncTimer(ISyncTimer* callback, int identifier, const std::string& name)
      : mSender(*this),
        mName(name),
        mCallback(callback),
        mIdentifier(identifier),
        mMilliseconds(0),
        mRepeat(false)
    {
    }

    /*!
     * @brief Start the timer
     * @param repeat True to repeat
     */
    void Start(int milliseconds, bool repeat)
    {
      mSignal.Fire();
      mMilliseconds = milliseconds;
      mRepeat = repeat;
      Thread::Start(mName);
    }

    /*!
     * @brief Stop the timer
     */
    void Cancel()
    {
      mSignal.Fire();
      Thread::Stop();
    }

    /*!
     * @brief Reset the timer: Stop and restart
     */
    void Reset()
    {
      Start(mMilliseconds, mRepeat);
    }
};

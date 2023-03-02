//
// Created by bkg2k on 02/03/23.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <utils/os/system/Signal.h>
#include <utils/storage/Queue.h>
#include <systems/SystemData.h>

class SystemHasher : private Thread
{
  public:
    //! Constructor
    SystemHasher();

    //! Destructor
    ~SystemHasher();

    //! Push a new system to auto-hash
    void Push(SystemData* system);

  private:
    //! Working signal
    Signal mSignal;
    //! Queue protector
    Mutex mLocker;
    //! System queue
    Queue<SystemData*> mQueue;

    /*!
     * @brief Check missing hashed and calculate them all
     * @param system System to check for missing hashes
     */
    void CheckMissingHashes(SystemData& system);

    /*
     * Thread implementation
     */

    /*!
     * @brief Break the thread asap
     */
    void Break() override { mSignal.Fire(); }

    /*!
     * @brief Process missing hashes in the queued systems
     */
    void Run() override;
};

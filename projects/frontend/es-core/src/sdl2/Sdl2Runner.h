//
// Created by bkg2k on 02/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/storage/HashMap.h>
#include <utils/storage/Array.h>
#include <utils/os/system/Mutex.h>
#include <utils/os/system/Signal.h>
#include "ISdl2EventNotifier.h"

class Sdl2Runner
{
  public:
    //! Constructor - Start the runner
    Sdl2Runner();

    //! Destructor - Shut down runner
    ~Sdl2Runner();

    /*!
     * @brief Register a notifier to receive the given event type
     * @param eventType Event type to register
     * @param notifier Interface that will receive the events
     */
    void Register(int eventType, ISdl2EventNotifier* notifier);

    /*!
     * @brief Unregister a notifier for the given event
     * @param eventType Event type to unregister
     * @param notifier Interface that was receiving this kind of event
     */
    void Unregister(int eventType, ISdl2EventNotifier* notifier);

    /*!
     * @brief Unregister all events for a given notifier
     * @param notifier Notifier to unregister
     */
    void UnregisterAll(ISdl2EventNotifier* notifier);

    //! Force the runner to stop
    void Stop();

    //! Main loop
    void Run();

  private:
    typedef Array<ISdl2EventNotifier*> NotifierArray;

    //! Registered events & notifiers
    HashMap<int, NotifierArray> mRegistered;
    //! Register hashmap syncher
    Mutex mLocker;

    //! Signal to synchronize thread init
    Signal mSignal;
};




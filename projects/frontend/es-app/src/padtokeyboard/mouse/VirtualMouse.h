//
// Created by bkg2k on 14/08/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/cplusplus/Bitflags.h>
#include <utils/os/system/Signal.h>
#include <utils/os/system/Thread.h>
#include <utils/storage/Queue.h>

class VirtualMouse final : public Thread
{
  public:
    //! Buttons
    enum class Buttons : short
    {
      None   = 0,
      Left   = 1,
      Middle = 2,
      Right  = 4,
    };

    enum class AxisAction : short
    {
      None       = 0,
      Accelerate = 1,
      Cancel     = 2,
    };

    struct EventAxis
    {
      short Axis;        //!< Move on axis
      AxisAction Action; //!< Action on axis

      bool IsMove() { return ((int)Axis | (int)Action) != 0; }
      void Reset() { Axis = 0; Action = AxisAction::None; }
    } __attribute__((packed));

    //! Compact event
    struct Event
    {
      EventAxis X;      //!< X axis event
      EventAxis Y;      //!< Y axis event
      Buttons ButtonsP; //!< Buttons pressed
      Buttons ButtonsR; //!< Buttons released

      bool IsMove() const { return ((int)ButtonsP | (int)ButtonsR) == 0; }
      void Reset() { ButtonsP = ButtonsR = Buttons::None; X.Reset(); Y.Reset(); }
    } __attribute__((packed));

    /*!
     * @brief Constructor
     */
    VirtualMouse();

    /*!
     * @brief Destructor
     */
    ~VirtualMouse() final;

    /*!
     * @brief Open the virtual keyboard /dev file
     */
    void Open();

    /*!
     * @brief Write an event (key press/release) to the virtual mouse
     * @param event Key and state
     */
    void Write(const Event& event);

    /*!
     * @brief Check virtual mouse readiness
     * @return True if the virtual mouse is ready
     */
    bool Ready() const { return (mFileDescriptor >= 0); }

    /*!
     * @brief Push an event to the queue
     * @param event Event
     */
    void PushEvent(const Event& event);

  private:
    //! Acceleration
    static constexpr int sMaxAcceleration = 16;

    //! Event queue
    Queue<Event> mEvents;

    //! Signal
    Signal mSignal;
    //! Syncher
    Mutex mSyncher;

    //! Current Event
    Event mEvent;
    //! Current event
    Event mNextEvent;

    //! Virtual mouse file descriptor
    int mFileDescriptor;

    /*!
     * @brief Close virtual mouse
     */
    void Close() const;

    /*!
     * @brief Emit low level event into the virtual mouse
     * @param type Event type
     * @param code Event code
     * @param value Event value
     */
    void Emit(int type, int code, int value) const;

    /*
     * Thread implementation
     */

    /*!
     * Calls the inheriting object's Run() method.
     * @note set fIsRunning false to exit
     */
    void Run() final;
};

DEFINE_BITFLAG_ENUM(VirtualMouse::Buttons, short)
DEFINE_BITFLAG_ENUM(VirtualMouse::AxisAction, short)


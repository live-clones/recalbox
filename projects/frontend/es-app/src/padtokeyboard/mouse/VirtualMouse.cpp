//
// Created by bkg2k on 14/08/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "VirtualMouse.h"
#include "linux/uinput.h"
#include "fcntl.h"
#include "memory.h"
#include "unistd.h"
#include "utils/Log.h"

VirtualMouse::VirtualMouse()
  : mEvent {}
  , mNextEvent {}
  , mFileDescriptor(-1)
{
  mEvent.Reset();
  Thread::Start("VMouse");
}

VirtualMouse::~VirtualMouse()
{
  Close();
  mSignal.Fire();
  Thread::Stop();
}

void VirtualMouse::Open()
{
  mFileDescriptor = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (mFileDescriptor < 0) return;

  // Enable all keys
  bool ok = (ioctl(mFileDescriptor, UI_SET_EVBIT, EV_KEY) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_KEYBIT, BTN_LEFT) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_KEYBIT, BTN_MIDDLE) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_KEYBIT, BTN_RIGHT) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_EVBIT, EV_REL) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_RELBIT, REL_X) == 0);
  if (ok) ok = (ioctl(mFileDescriptor, UI_SET_RELBIT, REL_Y) == 0);

  if (ok)
  {
    #if defined(UI_DEV_SETUP)
    struct uinput_setup usetup {};
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x4321; /* sample vendor */
    usetup.id.product = 0x8765; /* sample product */
    usetup.id.version = 1;
    strcpy(usetup.name, "VirtualMouse");

    ok = ioctl(mFileDescriptor, UI_DEV_SETUP, &usetup) == 0;
    #else
    struct uinput_user_dev udev {};
    memset(&udev, 0, sizeof(udev));
    udev.id.bustype = BUS_USB;
    udev.id.vendor = 0x4321; /* sample vendor */
    udev.id.product = 0x8765; /* sample product */
    udev.id.version = 1;
    strcpy(udev.name, "VirtualMouse");

    ok = write(mFileDescriptor, &udev, sizeof(udev)) == sizeof(udev);
    #endif
  }

  if (ok) ok = ioctl(mFileDescriptor, UI_DEV_CREATE) == 0;
  if (!ok) Close();
}

void VirtualMouse::Close()
{
  ioctl(mFileDescriptor, UI_DEV_DESTROY);
  close(mFileDescriptor);
  mFileDescriptor = -1;
}

void VirtualMouse::Emit(int type, int code, int value) const
{
  struct input_event event {};
  event.type = (unsigned short)type;
  event.code = (unsigned short)code;
  event.value = value;
  event.time.tv_sec = 0;
  event.time.tv_usec = 0;

  if (write(mFileDescriptor, &event, sizeof(event)) != sizeof(event))
  { LOG(LogError) << "[Pad2Keyboard] Error writing to virtual mouse"; }
}

void VirtualMouse::Write(const VirtualMouse::Event& event)
{
  // Button press
  if (event.ButtonsP != 0)
  {
    if ((event.ButtonsP & Buttons::Left  ) != 0) { Emit(EV_KEY, BTN_LEFT  , 1); Emit(EV_SYN, SYN_REPORT, 0); }
    if ((event.ButtonsP & Buttons::Middle) != 0) { Emit(EV_KEY, BTN_MIDDLE, 1); Emit(EV_SYN, SYN_REPORT, 0); }
    if ((event.ButtonsP & Buttons::Right ) != 0) { Emit(EV_KEY, BTN_RIGHT , 1); Emit(EV_SYN, SYN_REPORT, 0); }
  }
  // Button release
  if (event.ButtonsR != 0)
  {
    if ((event.ButtonsR & Buttons::Left  ) != 0) { Emit(EV_KEY, BTN_LEFT  , 0); Emit(EV_SYN, SYN_REPORT, 0); }
    if ((event.ButtonsR & Buttons::Middle) != 0) { Emit(EV_KEY, BTN_MIDDLE, 0); Emit(EV_SYN, SYN_REPORT, 0); }
    if ((event.ButtonsR & Buttons::Right ) != 0) { Emit(EV_KEY, BTN_RIGHT , 0); Emit(EV_SYN, SYN_REPORT, 0); }
  }

  // Moves
  if (event.X.Axis != 0) { Emit(EV_REL, REL_X, event.X.Axis); Emit(EV_SYN, SYN_REPORT, 0); }
  if (event.Y.Axis != 0) { Emit(EV_REL, REL_Y, event.Y.Axis); Emit(EV_SYN, SYN_REPORT, 0); }
}

void VirtualMouse::PushEvent(const VirtualMouse::Event& event)
{
  {
    Mutex::AutoLock locker(mSyncher);
    mNextEvent = event;
  }
  mSignal.Fire();
}

void VirtualMouse::Run()
{
  int sustain = 0; // Just wait for the first event
  while(IsRunning())
  {
    if (sustain == 0) mSignal.WaitSignal();
    else mSignal.WaitSignal(sustain); // Sustain mouse moves
    if (!IsRunning() || mFileDescriptor < 0) break; // Receive a close signal

    // Adjust event
    {
      Mutex::AutoLock locker(mSyncher);
      if (mNextEvent.X.IsMove()) mEvent.X = mNextEvent.X;
      if (mNextEvent.Y.IsMove()) mEvent.Y = mNextEvent.Y;
      mNextEvent.Reset();
    }
    // Sustain move
    sustain = INT32_MAX; // No more moves by default
    if (mEvent.X.Axis != 0)
    {
      sustain = 20;
      if (hasFlag(mEvent.X.Action, VirtualMouse::AxisAction::Accelerate))
      {
        if      (mEvent.X.Axis > 0 && mEvent.X.Axis <  sMaxAcceleration) mEvent.X.Axis++;
        else if (mEvent.X.Axis < 0 && mEvent.X.Axis > -sMaxAcceleration) mEvent.X.Axis--;
      }
    }
    if (mEvent.Y.Axis != 0)
    {
      sustain = 20;
      if (hasFlag(mEvent.Y.Action, VirtualMouse::AxisAction::Accelerate))
      {
        if      (mEvent.Y.Axis > 0 && mEvent.Y.Axis <  sMaxAcceleration) mEvent.Y.Axis++;
        else if (mEvent.Y.Axis < 0 && mEvent.Y.Axis > -sMaxAcceleration) mEvent.Y.Axis--;
      }
    }
    // Send event
    Write(mEvent);
    //{ LOG(LogWarning) << "Sustained Mouse! H:" << mEvent.X.Axis << " - V:" << mEvent.Y.Axis << " - B" << (int)mEvent.ButtonsP << ':' << (int)mEvent.ButtonsR; }
  }
}


//
// Created by bkg2k on 21/12/2019.
//
#include "VirtualKeyboard.h"
#include "linux/uinput.h"
#include "fcntl.h"
#include "memory.h"
#include "unistd.h"
#include "utils/Log.h"

VirtualKeyboard::VirtualKeyboard()
  : mFileDescriptor(-1)
{
}

VirtualKeyboard::~VirtualKeyboard()
{
  Close();
}

void VirtualKeyboard::Open()
{
  mFileDescriptor = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (mFileDescriptor < 0) return;

  // Enable all keys
  if (ioctl(mFileDescriptor, UI_SET_EVBIT, EV_KEY) == 0)
  {
    bool ok = true;
    for (int i = 256; --i >= 0;)
      if (ioctl(mFileDescriptor, UI_SET_KEYBIT, i) != 0) { ok = false; break; }

    if (ok)
    {
      #if defined(UI_DEV_SETUP)
        struct uinput_setup usetup {};
        memset(&usetup, 0, sizeof(usetup));
        usetup.id.bustype = BUS_USB;
        usetup.id.vendor = 0x1234; /* sample vendor */
        usetup.id.product = 0x5678; /* sample product */
        usetup.id.version = 1;
        strcpy(usetup.name, "VirtualKeyboard");

        ok = ioctl(mFileDescriptor, UI_DEV_SETUP, &usetup) == 0;
      #else
        struct uinput_user_dev udev {};
        memset(&udev, 0, sizeof(udev));
        udev.id.bustype = BUS_USB;
        udev.id.vendor = 0x1234; /* sample vendor */
        udev.id.product = 0x5678; /* sample product */
        udev.id.version = 1;
        strcpy(udev.name, "VirtualKeyboard");

        ok = write(mFileDescriptor, &udev, sizeof(udev)) == sizeof(udev);
      #endif
    }

    if (ok)
      ok = ioctl(mFileDescriptor, UI_DEV_CREATE) == 0;

    if (!ok) Close();
  }
}

void VirtualKeyboard::Close() const
{
  ioctl(mFileDescriptor, UI_DEV_DESTROY);
  close(mFileDescriptor);
}

void VirtualKeyboard::Emit(int type, int code, int value) const
{
  struct input_event event {};
  event.type = (unsigned short)type;
  event.code = (unsigned short)code;
  event.value = value;
  event.time.tv_sec = 0;
  event.time.tv_usec = 0;

  if (write(mFileDescriptor, &event, sizeof(event)) != sizeof(event))
  { LOG(LogError) << "[Pad2Keyboard] Error writting to virtual keyboard"; }
}

void VirtualKeyboard::Write(const VirtualKeyboard::Event& event)
{
  Emit(EV_KEY, event.Key, event.Pressed ? 1 : 0);
  Emit(EV_SYN, SYN_REPORT, 0);
}


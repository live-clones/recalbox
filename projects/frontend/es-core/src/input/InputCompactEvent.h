//
// Created by bkg2k on 28/10/2019.
//
#pragma once

#include <utils/cplusplus/Bitflags.h>
#include <input/InputEvent.h>
#include "RecalboxConf.h"

class InputDevice;

class InputCompactEvent
{
  public:
    enum class Entry: unsigned int
    {
      Nothing           = 0x00000000,
      Start             = 0x00000001,
      Select            = 0x00000002,
      Hotkey            = 0x00000004,
      A                 = 0x00000008,
      B                 = 0x00000010,
      X                 = 0x00000020,
      Y                 = 0x00000040,
      L1                = 0x00000080,
      R1                = 0x00000100,
      L2                = 0x00000200,
      R2                = 0x00000400,
      L3                = 0x00000800,
      R3                = 0x00001000,
      Up                = 0x00002000,
      Right             = 0x00004000,
      Down              = 0x00008000,
      Left              = 0x00010000,
      J1Left            = 0x00020000,
      J1Right           = 0x00040000,
      J1Up              = 0x00080000,
      J1Down            = 0x00100000,
      J2Left            = 0x00200000,
      J2Right           = 0x00400000,
      J2Up              = 0x00800000,
      J2Down            = 0x01000000,
      VolUp             = 0x02000000,
      VolDown           = 0x04000000,
      LumUp             = 0x08000000,
      LumDown           = 0x10000000,
      NeedConfiguration = 0x80000000, //!< Special case: When activated, this entry means the source device is not configured yet!
    };

  private:
    unsigned int mActivatedEntryFlags;   //!< Entries activated by this event, as bitflag (button pressed, joy moved, ...)
    unsigned int mDeactivatedEntryFlags; //!< Entries deactivated by this event, as bitflag (button released, joy centerted, ...)
    InputDevice& mInputDevice;           //!< Source Device
    InputEvent   mInputEvent;            //!< Original source event

  public:
    /*!
     * @brief Constructor
     * @param on Activated entries bitflag
     * @param off Deactivated entries bitflag
     * @param device Device instance from which the event is related to
     * @param originalEvent Original raw event
     */
    InputCompactEvent(Entry on, Entry off, InputDevice& device, const InputEvent originalEvent)
      : mActivatedEntryFlags((unsigned int)on),
        mDeactivatedEntryFlags((unsigned int)off),
        mInputDevice(device),
        mInputEvent(originalEvent)
    {
    }

    /*
     * Source accessors
     */

          InputDevice& Device()    const { return mInputDevice; }
    const InputEvent&  RawEvent()  const { return mInputEvent; }

    /*
     * Special accessors & keyboard
     */

    bool AskForConfiguration() const { return (mActivatedEntryFlags & (int)Entry::NeedConfiguration) != 0; }
    bool IsKeyboard()          const;
    bool KeyDown()             const;
    int  KeyCode()             const;
    bool KeyUp()               const;

    /*
     * Accessors w/o state
     */

    bool Up()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Up     ) != 0; }
    bool Down()           const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Down   ) != 0; }
    bool Left()           const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Left   ) != 0; }
    bool Right()          const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Right  ) != 0; }
    bool A()              const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::A      ) != 0; }
    bool B()              const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::B      ) != 0; }
    bool X()              const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::X      ) != 0; }
    bool Y()              const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Y      ) != 0; }
    bool Start()          const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Start  ) != 0; }
    bool Select()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Select ) != 0; }
    bool Hotkey()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::Hotkey ) != 0; }
    bool L1()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::L1     ) != 0; }
    bool R1()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::R1     ) != 0; }
    bool L2()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::L2     ) != 0; }
    bool R2()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::R2     ) != 0; }
    bool L3()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::L3     ) != 0; }
    bool R3()             const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::R3     ) != 0; }
    bool J1Up()           const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J1Up   ) != 0; }
    bool J1Down()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J1Down ) != 0; }
    bool J1Left()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J1Left ) != 0; }
    bool J1Right()        const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J1Right) != 0; }
    bool J2Up()           const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J2Up   ) != 0; }
    bool J2Down()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J2Down ) != 0; }
    bool J2Left()         const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J2Left ) != 0; }
    bool J2Right()        const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::J2Right) != 0; }
    bool VolumeUp()       const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::VolUp  ) != 0; }
    bool VolumeDown()     const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::VolDown) != 0; }
    bool BrightnessUp()   const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::LumUp  ) != 0; }
    bool BrightnessDown() const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & (int)Entry::LumDown) != 0; }

    /*
     * Simple accessors w/ state
     */

    bool UpPressed()              const { return (mActivatedEntryFlags   & (int)Entry::Up     ) != 0; }
    bool UpReleased()             const { return (mDeactivatedEntryFlags & (int)Entry::Up     ) != 0; }
    bool DownPressed()            const { return (mActivatedEntryFlags   & (int)Entry::Down   ) != 0; }
    bool DownReleased()           const { return (mDeactivatedEntryFlags & (int)Entry::Down   ) != 0; }
    bool LeftPressed()            const { return (mActivatedEntryFlags   & (int)Entry::Left   ) != 0; }
    bool LeftReleased()           const { return (mDeactivatedEntryFlags & (int)Entry::Left   ) != 0; }
    bool RightPressed()           const { return (mActivatedEntryFlags   & (int)Entry::Right  ) != 0; }
    bool RightReleased()          const { return (mDeactivatedEntryFlags & (int)Entry::Right  ) != 0; }
    bool APressed()               const { return (mActivatedEntryFlags   & (int)Entry::A      ) != 0; }
    bool AReleased()              const { return (mDeactivatedEntryFlags & (int)Entry::A      ) != 0; }
    bool BPressed()               const { return (mActivatedEntryFlags   & (int)Entry::B      ) != 0; }
    bool BReleased()              const { return (mDeactivatedEntryFlags & (int)Entry::B      ) != 0; }
    bool XPressed()               const { return (mActivatedEntryFlags   & (int)Entry::X      ) != 0; }
    bool XReleased()              const { return (mDeactivatedEntryFlags & (int)Entry::X      ) != 0; }
    bool YPressed()               const { return (mActivatedEntryFlags   & (int)Entry::Y      ) != 0; }
    bool YReleased()              const { return (mDeactivatedEntryFlags & (int)Entry::Y      ) != 0; }
    bool StartPressed()           const { return (mActivatedEntryFlags   & (int)Entry::Start  ) != 0; }
    bool StartReleased()          const { return (mDeactivatedEntryFlags & (int)Entry::Start  ) != 0; }
    bool SelectPressed()          const { return (mActivatedEntryFlags   & (int)Entry::Select ) != 0; }
    bool SelectReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::Select ) != 0; }
    bool HotkeyPressed()          const { return (mActivatedEntryFlags   & (int)Entry::Hotkey ) != 0; }
    bool HotkeyReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::Hotkey ) != 0; }
    bool L1Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::L1     ) != 0; }
    bool L1Released()             const { return (mDeactivatedEntryFlags & (int)Entry::L1     ) != 0; }
    bool R1Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::R1     ) != 0; }
    bool R1Released()             const { return (mDeactivatedEntryFlags & (int)Entry::R1     ) != 0; }
    bool L2Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::L2     ) != 0; }
    bool L2Released()             const { return (mDeactivatedEntryFlags & (int)Entry::L2     ) != 0; }
    bool R2Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::R2     ) != 0; }
    bool R2Released()             const { return (mDeactivatedEntryFlags & (int)Entry::R2     ) != 0; }
    bool L3Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::L3     ) != 0; }
    bool L3Released()             const { return (mDeactivatedEntryFlags & (int)Entry::L3     ) != 0; }
    bool R3Pressed()              const { return (mActivatedEntryFlags   & (int)Entry::R3     ) != 0; }
    bool R3Released()             const { return (mDeactivatedEntryFlags & (int)Entry::R3     ) != 0; }
    bool J1UpPressed()            const { return (mActivatedEntryFlags   & (int)Entry::J1Up   ) != 0; }
    bool J1UpReleased()           const { return (mDeactivatedEntryFlags & (int)Entry::J1Up   ) != 0; }
    bool J1DownPressed()          const { return (mActivatedEntryFlags   & (int)Entry::J1Down ) != 0; }
    bool J1DownReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::J1Down ) != 0; }
    bool J1LeftPressed()          const { return (mActivatedEntryFlags   & (int)Entry::J1Left ) != 0; }
    bool J1LeftReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::J1Left ) != 0; }
    bool J1RightPressed()         const { return (mActivatedEntryFlags   & (int)Entry::J1Right) != 0; }
    bool J1RightReleased()        const { return (mDeactivatedEntryFlags & (int)Entry::J1Right) != 0; }
    bool J2UpPressed()            const { return (mActivatedEntryFlags   & (int)Entry::J2Up   ) != 0; }
    bool J2UpReleased()           const { return (mDeactivatedEntryFlags & (int)Entry::J2Up   ) != 0; }
    bool J2DownPressed()          const { return (mActivatedEntryFlags   & (int)Entry::J2Down ) != 0; }
    bool J2DownReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::J2Down ) != 0; }
    bool J2LeftPressed()          const { return (mActivatedEntryFlags   & (int)Entry::J2Left ) != 0; }
    bool J2LeftReleased()         const { return (mDeactivatedEntryFlags & (int)Entry::J2Left ) != 0; }
    bool J2RightPressed()         const { return (mActivatedEntryFlags   & (int)Entry::J2Right) != 0; }
    bool J2RightReleased()        const { return (mDeactivatedEntryFlags & (int)Entry::J2Right) != 0; }
    bool VolumeUpPressed()        const { return (mActivatedEntryFlags   & (int)Entry::VolUp  ) != 0; }
    bool VolumeUpReleased()       const { return (mDeactivatedEntryFlags & (int)Entry::VolUp  ) != 0; }
    bool VolumeDownPressed()      const { return (mActivatedEntryFlags   & (int)Entry::VolDown) != 0; }
    bool VolumeDownReleased()     const { return (mDeactivatedEntryFlags & (int)Entry::VolDown) != 0; }
    bool BrightnessUpPressed()    const { return (mActivatedEntryFlags   & (int)Entry::LumUp  ) != 0; }
    bool BrightnessUpReleased()   const { return (mDeactivatedEntryFlags & (int)Entry::LumUp  ) != 0; }
    bool BrightnessDownPressed()  const { return (mActivatedEntryFlags   & (int)Entry::LumDown) != 0; }
    bool BrightnessDownReleased() const { return (mDeactivatedEntryFlags & (int)Entry::LumDown) != 0; }

    /*
     * Meta accessors
     */

    bool AnyUp()    const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & ((int)Entry::Up    | (int)Entry::J1Up    | (int)Entry::J2Up   )) != 0; }
    bool AnyDown()  const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & ((int)Entry::Down  | (int)Entry::J1Down  | (int)Entry::J2Down )) != 0; }
    bool AnyLeft()  const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & ((int)Entry::Left  | (int)Entry::J1Left  | (int)Entry::J2Left )) != 0; }
    bool AnyRight() const { return ((mActivatedEntryFlags | mDeactivatedEntryFlags) & ((int)Entry::Right | (int)Entry::J1Right | (int)Entry::J2Right)) != 0; }

    bool AnyUpPressed()     const { return (mActivatedEntryFlags   & ((int)Entry::Up    | (int)Entry::J1Up    | (int)Entry::J2Up   )) != 0; }
    bool AnyUpReleased()    const { return (mDeactivatedEntryFlags & ((int)Entry::Up    | (int)Entry::J1Up    | (int)Entry::J2Up   )) != 0; }
    bool AnyDownPressed()   const { return (mActivatedEntryFlags   & ((int)Entry::Down  | (int)Entry::J1Down  | (int)Entry::J2Down )) != 0; }
    bool AnyDownReleased()  const { return (mDeactivatedEntryFlags & ((int)Entry::Down  | (int)Entry::J1Down  | (int)Entry::J2Down )) != 0; }
    bool AnyLeftPressed()   const { return (mActivatedEntryFlags   & ((int)Entry::Left  | (int)Entry::J1Left  | (int)Entry::J2Left )) != 0; }
    bool AnyLeftReleased()  const { return (mDeactivatedEntryFlags & ((int)Entry::Left  | (int)Entry::J1Left  | (int)Entry::J2Left )) != 0; }
    bool AnyRightPressed()  const { return (mActivatedEntryFlags   & ((int)Entry::Right | (int)Entry::J1Right | (int)Entry::J2Right)) != 0; }
    bool AnyRightReleased() const { return (mDeactivatedEntryFlags & ((int)Entry::Right | (int)Entry::J1Right | (int)Entry::J2Right)) != 0; }

    bool AnythingPressed()  const { return mActivatedEntryFlags   != 0; }
    bool AnythingReleased() const { return mDeactivatedEntryFlags != 0; }

    bool AnyButtonPressed()  const { return (mActivatedEntryFlags   & ((int)Entry::A | (int)Entry::B | (int)Entry::X | (int)Entry::Y | (int)Entry::R1 | (int)Entry::L1 | (int)Entry::R2 | (int)Entry::L2 | (int)Entry::Start | (int)Entry::Select | (int)Entry::VolUp | (int)Entry::VolDown | (int)Entry::LumDown | (int)Entry::LumUp)) != 0; }
    bool AnyButtonReleased() const { return (mDeactivatedEntryFlags & ((int)Entry::A | (int)Entry::B | (int)Entry::X | (int)Entry::Y | (int)Entry::R1 | (int)Entry::L1 | (int)Entry::R2 | (int)Entry::L2 | (int)Entry::Start | (int)Entry::Select | (int)Entry::VolUp | (int)Entry::VolDown | (int)Entry::LumDown | (int)Entry::LumUp)) != 0; }

    bool Empty() const
    {
      return ((mDeactivatedEntryFlags | mActivatedEntryFlags) == 0) && (!KeyDown() && !KeyUp());
    }

    bool ValidPressed() const { return RecalboxConf::Instance().GetSwapValidateAndCancel() ? APressed() : BPressed(); }
    bool ValidReleased() const { return RecalboxConf::Instance().GetSwapValidateAndCancel() ? AReleased() : BReleased(); }
    bool CancelPressed() const { return RecalboxConf::Instance().GetSwapValidateAndCancel() ? BPressed() : APressed(); }
    bool CancelReleased() const { return RecalboxConf::Instance().GetSwapValidateAndCancel() ? BReleased() : AReleased(); }

    /*
     * Debug
     */

    std::string ToString() const;
};

DEFINE_BITFLAG_ENUM(InputCompactEvent::Entry, unsigned int)

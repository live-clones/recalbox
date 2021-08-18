//
// Created by bkg2k on 21/12/2019.
//

#include <linux/input.h>
#include <utils/os/fs/Path.h>
#include <utils/Files.h>
#include <utils/Strings.h>
#include <utils/Log.h>
#include <cstring>
#include <padtokeyboard/mouse/VirtualMouse.h>
#include "MappingConfiguration.h"

#define __STRL(x) x, sizeof(x)-1

MappingConfiguration::Mapping::Mapping()
  : mType {}
  , mCount {}
  , mDelay {}
  , mCodes {}
{
  memset(mCodes, 0, sizeof(mCodes));
  memset(mDelay, 0, sizeof(mDelay));
  memset(mCount, 0, sizeof(mCount));
  for(int i = (int)PadItems::__Count; --i >= 0; )
    mType[i] = Types::None;
}

bool MappingConfiguration::Mapping::Valid() const
{
  for(int i = (int)PadItems::__Count; --i >= 0; )
    if (mType[i] != Types::None)
      return true;
  return false;
}

int MappingConfiguration::Mapping::Count() const
{
  int count = 0;
  for(int i = (int)PadItems::__Count; --i >= 0; )
    if (mType[i] != Types::None)
      count++;
  return count;
}

MappingConfiguration::MappingConfiguration(const Path& romPath)
{
  Load(romPath);
}

void MappingConfiguration::Load(const Path& path, bool folder)
{
  // Recurse to the root
  if (path.IsEmpty()) return;
  // Recurse first to process folder in descending order
  Load(path.Directory(), true);

  Path configurationPath =
    folder ?
    path / ConfigurationExtensions :                                  // /path/to/folder/.p2k.cfg
    path.ChangeExtension(path.Extension() + ConfigurationExtensions); // /path/to/file.ext.p2k.cfg

  // Load whole file
  std::string content = Files::LoadFile(configurationPath);
  if (content.empty()) return;

  // Line loop
  int start = 0;
  while(start < (int)content.length())
  {
    int end = (int)content.find('\n', start);
    if (end < 0) end = (int)content.length();
    int equal = (int)content.find('=', start);
    if (equal > start && equal < end)
    {
      // Extract and trim key/values
      std::string key = Strings::Trim(content.substr(start, equal - start), " \t\r\n");
      std::string value = Strings::Trim(content.substr(equal + 1, end - (equal + 1)), " \t\r\n");
      // Extract comment if any
      std::string comment;
      int commentPos = (int)value.find(";;");
      if (commentPos != (int)std::string::npos)
      {
        comment = Strings::Trim(value.substr(commentPos + 2), " \t\r\n");
        value = Strings::Trim(value.erase(commentPos, UINT32_MAX), " \t\r\n");
      }
      // Store mapping
      if (key.length() > 0)
        if (key[0] != '#' && key[0] != ';')
          AssignMapping(Strings::ToLowerASCII(key), Strings::ToLowerASCII(value), comment);
    }
    start = end + 1;
  }
}

void MappingConfiguration::AssignMapping(const std::string& key, const std::string& value, const std::string& comment)
{
  PadItems padItem = PadItems::Invalid;
  int padNum = 0;
  short Codes[VirtualKeyboard::sMax];
  Types type = Types::None;
  int count = 0;
  int delay = 0;

  // Normal parsing
  if (ParsePadItems(key, padNum, padItem))
    if (ParseKeyCode(value, Codes, type, count, delay))
      if (padNum < Input::sMaxInputDevices)
      {
        mMapping[padNum].Assign(padItem, type, Codes, count, delay, comment);
        return;
      }

  // Shortcuts
  if (value == "mouse.moves")
  {
    // Check format X:key
    if (key.length() > 2)
      if ((unsigned int)key[0] - 0x30u <= 9)
        if (key[1] == ':')
        {
          padNum = key[0] - 0x30;
          memset(Codes, 0, sizeof(Codes));
          if (strcmp(key.data() + 2, "j1") == 0)
          {
            Codes[0] = (short)MouseMove::Left;  mMapping[padNum].Assign(PadItems::J1Left , Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Right; mMapping[padNum].Assign(PadItems::J1Right, Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Up;    mMapping[padNum].Assign(PadItems::J1Up   , Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Down;  mMapping[padNum].Assign(PadItems::J1Down , Types::MouseMove, Codes, 1, 0, comment);
            return;
          }
          if (strcmp(key.data() + 2, "j2") == 0)
          {
            Codes[0] = (short)MouseMove::Left;  mMapping[padNum].Assign(PadItems::J2Left , Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Right; mMapping[padNum].Assign(PadItems::J2Right, Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Up;    mMapping[padNum].Assign(PadItems::J2Up   , Types::MouseMove, Codes, 1, 0, comment);
            Codes[0] = (short)MouseMove::Down;  mMapping[padNum].Assign(PadItems::J2Down , Types::MouseMove, Codes, 1, 0, comment);
            return;
          }
        }
  }

  { LOG(LogWarning) << "[Pad2Keyboard] Unknown key or value: " << key << " = " << value; }
}

MappingConfiguration::Types MappingConfiguration::Translate(Pad::Event& event, VirtualKeyboard::EventList& keyboard, VirtualMouse::Event& mouse) const
{
  // Invalid
  if ((unsigned int)event.Pad >= Input::sMaxInputDevices) return Types::None;

  const Mapping& mapping = mMapping[(unsigned char)event.Pad];
  switch(mapping.Type(event.Item))
  {
    case Types::None: return Types::None;
    case Types::Keyboard:
    case Types::KeyboardSequence:
    {
      if (!event.Analog)
      {
        keyboard.Count = mapping.Count(event.Item);
        keyboard.Delay = mapping.Delay(event.Item);
        for(int i = keyboard.Count; --i >= 0; )
        {
          keyboard.Events[i].Key = (short)mapping.Code(event.Item, i);
          keyboard.Events[i].Pressed = event.On;
        }
        return mMapping[(unsigned char)event.Pad].Type(event.Item);
      }
      break;
    }
    case Types::MouseButton:
    {
      mouse.Reset();
      if (!event.Analog)
      {
        VirtualMouse::Buttons button = VirtualMouse::Buttons::None;
        switch(mapping.Code(event.Item, 0))
        {
          case BTN_LEFT: button = VirtualMouse::Buttons::Left; break;
          case BTN_MIDDLE: button = VirtualMouse::Buttons::Middle; break;
          case BTN_RIGHT: button = VirtualMouse::Buttons::Right; break;
          default: break;
        }
        mouse.ButtonsP = event.On ? button : VirtualMouse::Buttons::None;
        mouse.ButtonsR = !event.On ? button : VirtualMouse::Buttons::None;
        return Types::MouseButton;
      }
      break;
    }
    case Types::MouseMove:
    {
      mouse.Reset();
      if (event.Analog)
      {
        VirtualMouse::AxisAction cancel = VirtualMouse::AxisAction::Cancel;
        switch((MouseMove)mapping.Code(event.Item, 0))
        {
          case MouseMove::Left : mouse.X.Axis = (short)-event.Value; if (event.Value == 0) mouse.X.Action = cancel; break;
          case MouseMove::Right: mouse.X.Axis = (short) event.Value; if (event.Value == 0) mouse.X.Action = cancel; break;
          case MouseMove::Up   : mouse.Y.Axis = (short)-event.Value; if (event.Value == 0) mouse.Y.Action = cancel; break;
          case MouseMove::Down : mouse.Y.Axis = (short) event.Value; if (event.Value == 0) mouse.Y.Action = cancel; break;
          default: break;
        }
        return Types::MouseMove;
      }
      else if (event.Item < PadItems::J1Up || event.Item > PadItems::J2Left) // Ignore numeric values from axis
      {
        VirtualMouse::AxisAction accelerate = VirtualMouse::AxisAction::Accelerate;
        switch((MouseMove)mapping.Code(event.Item, 0))
        {
          case MouseMove::Left : mouse.X.Axis = event.On ? -1 : 0; if (!event.On) accelerate |= VirtualMouse::AxisAction::Cancel; mouse.X.Action = accelerate; break;
          case MouseMove::Right: mouse.X.Axis = event.On ?  1 : 0; if (!event.On) accelerate |= VirtualMouse::AxisAction::Cancel; mouse.X.Action = accelerate; break;
          case MouseMove::Up   : mouse.Y.Axis = event.On ? -1 : 0; if (!event.On) accelerate |= VirtualMouse::AxisAction::Cancel; mouse.Y.Action = accelerate; break;
          case MouseMove::Down : mouse.Y.Axis = event.On ?  1 : 0; if (!event.On) accelerate |= VirtualMouse::AxisAction::Cancel; mouse.Y.Action = accelerate; break;
          default: break;
        }
        return Types::MouseMove;
      }
    }
  }
  return Types::None;
}

bool MappingConfiguration::Valid() const
{
  for(int i = Input::sMaxInputDevices; --i >= 0; )
    if (mMapping[i].Valid())
      return true;
  return false;
}

int MappingConfiguration::Count() const
{
  int count = 0;
  for(int device = Input::sMaxInputDevices; --device >= 0; )
    count += mMapping[device].Count();
  return count;
}

bool MappingConfiguration::ParseKeyCode(const std::string& value, Mapping::CodeArray codes, Types& type, int& count, int& delay)
{
  static const struct KeyNameTranslator
  {
    const char* KeyName;
    short KeyNameLength;
    short KeyCode;
    Types Type;
  }
  Translators[] =
  {
    // Keyboard
    { __STRL("")           , 0             , Types::Keyboard },
    { __STRL("esc")        , KEY_ESC       , Types::Keyboard },
    { __STRL("1")          , KEY_1         , Types::Keyboard },
    { __STRL("2")          , KEY_2         , Types::Keyboard },
    { __STRL("3")          , KEY_3         , Types::Keyboard },
    { __STRL("4")          , KEY_4         , Types::Keyboard },
    { __STRL("5")          , KEY_5         , Types::Keyboard },
    { __STRL("6")          , KEY_6         , Types::Keyboard },
    { __STRL("7")          , KEY_7         , Types::Keyboard },
    { __STRL("8")          , KEY_8         , Types::Keyboard },
    { __STRL("9")          , KEY_9         , Types::Keyboard },
    { __STRL("0")          , KEY_0         , Types::Keyboard },
    { __STRL("backspace")  , KEY_BACKSPACE , Types::Keyboard },
    { __STRL("tab")        , KEY_TAB       , Types::Keyboard },
    { __STRL("q")          , KEY_Q         , Types::Keyboard },
    { __STRL("w")          , KEY_W         , Types::Keyboard },
    { __STRL("e")          , KEY_E         , Types::Keyboard },
    { __STRL("r")          , KEY_R         , Types::Keyboard },
    { __STRL("t")          , KEY_T         , Types::Keyboard },
    { __STRL("y")          , KEY_Y         , Types::Keyboard },
    { __STRL("u")          , KEY_U         , Types::Keyboard },
    { __STRL("i")          , KEY_I         , Types::Keyboard },
    { __STRL("o")          , KEY_O         , Types::Keyboard },
    { __STRL("p")          , KEY_P         , Types::Keyboard },
    { __STRL("enter")      , KEY_ENTER     , Types::Keyboard },
    { __STRL("leftctrl")   , KEY_LEFTCTRL  , Types::Keyboard },
    { __STRL("a")          , KEY_A         , Types::Keyboard },
    { __STRL("s")          , KEY_S         , Types::Keyboard },
    { __STRL("d")          , KEY_D         , Types::Keyboard },
    { __STRL("f")          , KEY_F         , Types::Keyboard },
    { __STRL("g")          , KEY_G         , Types::Keyboard },
    { __STRL("h")          , KEY_H         , Types::Keyboard },
    { __STRL("j")          , KEY_J         , Types::Keyboard },
    { __STRL("k")          , KEY_K         , Types::Keyboard },
    { __STRL("l")          , KEY_L         , Types::Keyboard },
    { __STRL("leftshift")  , KEY_LEFTSHIFT , Types::Keyboard },
    { __STRL("z")          , KEY_Z         , Types::Keyboard },
    { __STRL("x")          , KEY_X         , Types::Keyboard },
    { __STRL("c")          , KEY_C         , Types::Keyboard },
    { __STRL("v")          , KEY_V         , Types::Keyboard },
    { __STRL("b")          , KEY_B         , Types::Keyboard },
    { __STRL("n")          , KEY_N         , Types::Keyboard },
    { __STRL("m")          , KEY_M         , Types::Keyboard },
    { __STRL("rightshift") , KEY_RIGHTSHIFT, Types::Keyboard },
    { __STRL("kpasterisk") , KEY_KPASTERISK, Types::Keyboard },
    { __STRL("leftalt")    , KEY_LEFTALT   , Types::Keyboard },
    { __STRL("space")      , KEY_SPACE     , Types::Keyboard },
    { __STRL("f1")         , KEY_F1        , Types::Keyboard },
    { __STRL("f2")         , KEY_F2        , Types::Keyboard },
    { __STRL("f3")         , KEY_F3        , Types::Keyboard },
    { __STRL("f4")         , KEY_F4        , Types::Keyboard },
    { __STRL("f5")         , KEY_F5        , Types::Keyboard },
    { __STRL("f6")         , KEY_F6        , Types::Keyboard },
    { __STRL("f7")         , KEY_F7        , Types::Keyboard },
    { __STRL("f8")         , KEY_F8        , Types::Keyboard },
    { __STRL("f9")         , KEY_F9        , Types::Keyboard },
    { __STRL("f10")        , KEY_F10       , Types::Keyboard },
    { __STRL("kp7")        , KEY_KP7       , Types::Keyboard },
    { __STRL("kp8")        , KEY_KP8       , Types::Keyboard },
    { __STRL("kp9")        , KEY_KP9       , Types::Keyboard },
    { __STRL("kpminus")    , KEY_KPMINUS   , Types::Keyboard },
    { __STRL("kp4")        , KEY_KP4       , Types::Keyboard },
    { __STRL("kp5")        , KEY_KP5       , Types::Keyboard },
    { __STRL("kp6")        , KEY_KP6       , Types::Keyboard },
    { __STRL("kpplus")     , KEY_KPPLUS    , Types::Keyboard },
    { __STRL("kp1")        , KEY_KP1       , Types::Keyboard },
    { __STRL("kp2")        , KEY_KP2       , Types::Keyboard },
    { __STRL("kp3")        , KEY_KP3       , Types::Keyboard },
    { __STRL("kp0")        , KEY_KP0       , Types::Keyboard },
    { __STRL("kpdot")      , KEY_KPDOT     , Types::Keyboard },
    { __STRL("f11")        , KEY_F11       , Types::Keyboard },
    { __STRL("f12")        , KEY_F12       , Types::Keyboard },
    { __STRL("kpenter")    , KEY_KPENTER   , Types::Keyboard },
    { __STRL("rightctrl")  , KEY_RIGHTCTRL , Types::Keyboard },
    { __STRL("kpslash")    , KEY_KPSLASH   , Types::Keyboard },
    { __STRL("rightalt")   , KEY_RIGHTALT  , Types::Keyboard },
    { __STRL("home")       , KEY_HOME      , Types::Keyboard },
    { __STRL("up")         , KEY_UP        , Types::Keyboard },
    { __STRL("pageup")     , KEY_PAGEUP    , Types::Keyboard },
    { __STRL("left")       , KEY_LEFT      , Types::Keyboard },
    { __STRL("right")      , KEY_RIGHT     , Types::Keyboard },
    { __STRL("end")        , KEY_END       , Types::Keyboard },
    { __STRL("down")       , KEY_DOWN      , Types::Keyboard },
    { __STRL("pagedown")   , KEY_PAGEDOWN  , Types::Keyboard },
    { __STRL("insert")     , KEY_INSERT    , Types::Keyboard },
    { __STRL("delete")     , KEY_DELETE    , Types::Keyboard },
    { __STRL("s01")        , KEY_GRAVE     , Types::Keyboard },
    { __STRL("s02")        , KEY_MINUS     , Types::Keyboard },
    { __STRL("s03")        , KEY_EQUAL     , Types::Keyboard },
    { __STRL("s04")        , KEY_LEFTBRACE , Types::Keyboard },
    { __STRL("s05")        , KEY_RIGHTBRACE, Types::Keyboard },
    { __STRL("s06")        , KEY_SEMICOLON , Types::Keyboard },
    { __STRL("s07")        , KEY_APOSTROPHE, Types::Keyboard },
    { __STRL("s08")        , KEY_BACKSLASH , Types::Keyboard },
    { __STRL("s09")        , KEY_COMMA     , Types::Keyboard },
    { __STRL("s10")        , KEY_DOT       , Types::Keyboard },
    { __STRL("s11")        , KEY_SLASH     , Types::Keyboard },
    // Mouse button
    { __STRL("mouse.button.left")  , BTN_LEFT     , Types::MouseButton },
    { __STRL("mouse.button.middle"), BTN_MIDDLE   , Types::MouseButton },
    { __STRL("mouse.button.right") , BTN_RIGHT    , Types::MouseButton },
    // Mouse move
    { __STRL("mouse.move.up")       , (int)MouseMove::Up            , Types::MouseMove  },
    { __STRL("mouse.move.down")     , (int)MouseMove::Down          , Types::MouseMove  },
    { __STRL("mouse.move.left")     , (int)MouseMove::Left          , Types::MouseMove  },
    { __STRL("mouse.move.right")    , (int)MouseMove::Right         , Types::MouseMove  },
  };

  memset(codes, 0, sizeof(Mapping::CodeArray));

  bool isMulti = value.find(' ') !=  std::string::npos;

  if (!isMulti) // Simple c& most common case
  {
    // Lookup keycode
    for(int i = (int)(sizeof(Translators) / sizeof(KeyNameTranslator)); --i >= 0;)
    {
      const KeyNameTranslator& translator = Translators[i];
      if (translator.KeyNameLength == (int)value.length())
        if (strncmp(value.c_str(), translator.KeyName, translator.KeyNameLength) == 0)
        {
          count = 1;
          delay = 0;
          codes[0] = translator.KeyCode;
          type = translator.Type;
          return true;
        }
    }
  }
  else
    for(;;)
    {
      // Split
      Strings::Vector list = Strings::Split(value, ' ', true);

      // Check type & delay
      int index = 0;
      type = Types::Keyboard;
      delay = 0;
      std::string& intro = list[0];
      if (Strings::StartsWith(intro, "+", 1))
      {
        if (!Strings::ToInt(intro, 1, delay)) break;
        index++;
      }
      else if (Strings::StartsWith(intro, "/", 1))
      {
        type = Types::KeyboardSequence;
        if (!Strings::ToInt(intro, 1, delay)) break;
        index++;
      }

      // Lookup keycode
      count = 0;
      bool ok = true;
      for(; index < (int)list.size(); ++index)
      {
        bool found = false;
        for(int i = (int)(sizeof(Translators) / sizeof(KeyNameTranslator)); --i >= 0;)
        {
          const KeyNameTranslator& translator = Translators[i];
          if (translator.KeyNameLength == (int)list[index].length())
            if (strncmp(list[index].c_str(), translator.KeyName, translator.KeyNameLength) == 0)
            {
              if (translator.Type != Types::Keyboard) { LOG(LogError) << "[Pad2Keyboard] Mouse mapping not allowed in key list."; break; }
              if (count >= VirtualKeyboard::sMax) break; // No more!
              codes[count++] = translator.KeyCode;
              found = true;
              break; // exit inner loop when found
            }
        }
        if (!found) { ok = false; break; }; // exit outer loop if we have an unknown key
      }

      if (ok) return true;
      break;
    }

  return false;
}

bool MappingConfiguration::ParsePadItems(const std::string& value, int& padNum, PadItems& padItem)
{
  static const struct PadItemTranslator
  {
    const char* TextItem;
    short TextItemLength;
    PadItems PadItem;
  } __attribute__((packed))
  Translators[] =
  {
    { __STRL("up"     ), PadItems::Up      }, //!< DPad Up direction
    { __STRL("right"  ), PadItems::Right   }, //!< DPad Right direction
    { __STRL("down"   ), PadItems::Down    }, //!< DPad Down direction
    { __STRL("left"   ), PadItems::Left    }, //!< DPad Left direction
    { __STRL("a"      ), PadItems::A       }, //!< A Button
    { __STRL("b"      ), PadItems::B       }, //!< B Button
    { __STRL("x"      ), PadItems::X       }, //!< X Button
    { __STRL("y"      ), PadItems::Y       }, //!< Y Button
    { __STRL("l1"     ), PadItems::L1      }, //!< L1 Button/Trigger
    { __STRL("r1"     ), PadItems::R1      }, //!< R1 Button/Trigger
    { __STRL("l2"     ), PadItems::L2      }, //!< L2 Button/Trigger
    { __STRL("r2"     ), PadItems::R2      }, //!< R2 Button/Trigger
    { __STRL("l3"     ), PadItems::L3      }, //!< L3 Button/Trigger
    { __STRL("r3"     ), PadItems::R3      }, //!< R3 Button/Trigger
    { __STRL("start"  ), PadItems::Start   }, //!< Start button
    { __STRL("select" ), PadItems::Select  }, //!< Select button
    { __STRL("hotkey" ), PadItems::Hotkey  }, //!< Hotkey (or Home) button
    { __STRL("j1up"   ), PadItems::J1Up    }, //!< Joystick 1 (left) Up position
    { __STRL("j1right"), PadItems::J1Right }, //!< Joystick 1 (left) Right position
    { __STRL("j1down" ), PadItems::J1Down  }, //!< Joystick 1 (left) Down position
    { __STRL("j1left" ), PadItems::J1Left  }, //!< Joystick 1 (left) Left position
    { __STRL("j2up"   ), PadItems::J2Up    }, //!< Joystick 2 (right) Uo positio
    { __STRL("j2right"), PadItems::J2Right }, //!< Joystick 2 (right) Right position
    { __STRL("j2down" ), PadItems::J2Down  }, //!< Joystick 2 (right) Down position
    { __STRL("j2left" ), PadItems::J2Left  }, //!< Joystick 2 (right) Left position
  };
  
  // Check format X:key
  if (value.length() < 3) return false;
  if ((unsigned int)value[0] - 0x30u > 9) return false;
  if (value[1] != ':') return false;

  // Store pad number
  padNum = value[0] - 0x30;

  // Lookup key
  for(int i = (int)(sizeof(Translators) / sizeof(PadItemTranslator)); --i >= 0;)
  {
    const PadItemTranslator& translator = Translators[i];
    if (translator.TextItemLength == (int)(value.length() - 2))
      if (strncmp(value.c_str() + 2, translator.TextItem, translator.TextItemLength) == 0)
      {
        padItem = translator.PadItem;
        return true;
      }
  }

  return false;
}

const MappingConfiguration::Hints& MappingConfiguration::HintList() const
{
  static Hints result;
  result.clear();

  for(int device = 0; device < Input::sMaxInputDevices; ++device )
  {
    const Mapping& mapping = mMapping[device];
    for (int index = 0; index < (int) PadItems::__Count; ++index)
      if (mapping.Type((PadItems)index) != Types::None)
        result.push_back({{ device, (PadItems)index }, &mapping.Comment((PadItems)index) });
  }

  return result;
}



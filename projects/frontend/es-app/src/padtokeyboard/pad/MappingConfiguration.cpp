//
// Created by bkg2k on 21/12/2019.
//

#include <linux/input.h>
#include <utils/os/fs/Path.h>
#include <utils/Files.h>
#include <utils/Strings.h>
#include <utils/Log.h>
#include <cstring>
#include "MappingConfiguration.h"
#include "../keyboard/VirtualKeyboard.h"

#define __STRL(x) x, sizeof(x)-1

MappingConfiguration::Mapping::Mapping()
  : PadItemToKeyCodes {}
{
  for(int i = (int)PadItems::__Count; --i >= 0; )
    PadItemToKeyCodes[i] = 0;
}

bool MappingConfiguration::Mapping::Valid() const
{
  for(int i = (int)PadItems::__Count; --i >= 0; )
    if (PadItemToKeyCodes[i] != 0)
      return true;
  return false;
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
    if (end < 0) end = content.length();
    int equal = (int)content.find('=', start);
    if (equal >= 0 && equal < end)
    {
      // Extract and trim key/values
      std::string key = Strings::ToLowerASCII(Strings::Trim(content.substr(start, equal - start), " \t\r\n"));
      std::string value = Strings::ToLowerASCII(Strings::Trim(content.substr(equal + 1, end - (equal + 1)), " \t\r\n"));
      if (key.length() > 0)
      {
        if (key[0] != '#' && key[0] != ';')
          AssignMapping(key, value);
      }
    }
    start = end + 1;
  }
}

void MappingConfiguration::AssignMapping(const std::string& key, const std::string& value)
{
  PadItems padItem = PadItems::Invalid;
  int padNum = 0;
  int keyCode = 0;

  if (ParsePadItems(key, padNum, padItem))
    if (ParseKeyCode(value, keyCode))
      if (padNum < Input::sMaxInputDevices)
        mMapping[padNum].PadItemToKeyCodes[(int)padItem] = keyCode;
}

VirtualKeyboard::Event MappingConfiguration::Translate(Pad::Event& event) const
{
  if ((unsigned int)event.Pad >= Input::sMaxInputDevices) return { 0, false };
  return { (short)mMapping[(int)event.Pad].PadItemToKeyCodes[(int)event.Item], event.On };
}

bool MappingConfiguration::Valid() const
{
  for(int i = Input::sMaxInputDevices; --i >= 0; )
    if (mMapping[i].Valid())
      return true;
  return false;
}

bool MappingConfiguration::ParseKeyCode(const std::string& value, int& code)
{
  static const struct KeyNameTranslator
  {
    const char* KeyName;
    short KeyNameLength;
    short KeyCode;
  }
  Translators[] =
  {
    { __STRL("")           , 0        },
    { __STRL("esc")        , KEY_ESC        },
    { __STRL("1")          , KEY_1          },
    { __STRL("2")          , KEY_2          },
    { __STRL("3")          , KEY_3          },
    { __STRL("4")          , KEY_4          },
    { __STRL("5")          , KEY_5          },
    { __STRL("6")          , KEY_6          },
    { __STRL("7")          , KEY_7          },
    { __STRL("8")          , KEY_8          },
    { __STRL("9")          , KEY_9          },
    { __STRL("0")          , KEY_0          },
    { __STRL("backspace")  , KEY_BACKSPACE  },
    { __STRL("tab")        , KEY_TAB        },
    { __STRL("q")          , KEY_Q          },
    { __STRL("w")          , KEY_W          },
    { __STRL("e")          , KEY_E          },
    { __STRL("r")          , KEY_R          },
    { __STRL("t")          , KEY_T          },
    { __STRL("y")          , KEY_Y          },
    { __STRL("u")          , KEY_U          },
    { __STRL("i")          , KEY_I          },
    { __STRL("o")          , KEY_O          },
    { __STRL("p")          , KEY_P          },
    { __STRL("enter")      , KEY_ENTER      },
    { __STRL("leftctrl")   , KEY_LEFTCTRL   },
    { __STRL("a")          , KEY_A          },
    { __STRL("s")          , KEY_S          },
    { __STRL("d")          , KEY_D          },
    { __STRL("f")          , KEY_F          },
    { __STRL("g")          , KEY_G          },
    { __STRL("h")          , KEY_H          },
    { __STRL("j")          , KEY_J          },
    { __STRL("k")          , KEY_K          },
    { __STRL("l")          , KEY_L          },
    { __STRL("leftshift")  , KEY_LEFTSHIFT  },
    { __STRL("z")          , KEY_Z          },
    { __STRL("x")          , KEY_X          },
    { __STRL("c")          , KEY_C          },
    { __STRL("v")          , KEY_V          },
    { __STRL("b")          , KEY_B          },
    { __STRL("n")          , KEY_N          },
    { __STRL("m")          , KEY_M          },
    { __STRL("rightshift") , KEY_RIGHTSHIFT },
    { __STRL("kpasterisk") , KEY_KPASTERISK },
    { __STRL("leftalt")    , KEY_LEFTALT    },
    { __STRL("space")      , KEY_SPACE      },
    { __STRL("f1")         , KEY_F1         },
    { __STRL("f2")         , KEY_F2         },
    { __STRL("f3")         , KEY_F3         },
    { __STRL("f4")         , KEY_F4         },
    { __STRL("f5")         , KEY_F5         },
    { __STRL("f6")         , KEY_F6         },
    { __STRL("f7")         , KEY_F7         },
    { __STRL("f8")         , KEY_F8         },
    { __STRL("f9")         , KEY_F9         },
    { __STRL("f10")        , KEY_F10        },
    { __STRL("kp7")        , KEY_KP7        },
    { __STRL("kp8")        , KEY_KP8        },
    { __STRL("kp9")        , KEY_KP9        },
    { __STRL("kpminus")    , KEY_KPMINUS    },
    { __STRL("kp4")        , KEY_KP4        },
    { __STRL("kp5")        , KEY_KP5        },
    { __STRL("kp6")        , KEY_KP6        },
    { __STRL("kpplus")     , KEY_KPPLUS     },
    { __STRL("kp1")        , KEY_KP1        },
    { __STRL("kp2")        , KEY_KP2        },
    { __STRL("kp3")        , KEY_KP3        },
    { __STRL("kp0")        , KEY_KP0        },
    { __STRL("kpdot")      , KEY_KPDOT      },
    { __STRL("f11")        , KEY_F11        },
    { __STRL("f12")        , KEY_F12        },
    { __STRL("kpenter")    , KEY_KPENTER    },
    { __STRL("rightctrl")  , KEY_RIGHTCTRL  },
    { __STRL("kpslash")    , KEY_KPSLASH    },
    { __STRL("rightalt")   , KEY_RIGHTALT   },
    { __STRL("home")       , KEY_HOME       },
    { __STRL("up")         , KEY_UP         },
    { __STRL("pageup")     , KEY_PAGEUP     },
    { __STRL("left")       , KEY_LEFT       },
    { __STRL("right")      , KEY_RIGHT      },
    { __STRL("end")        , KEY_END        },
    { __STRL("down")       , KEY_DOWN       },
    { __STRL("pagedown")   , KEY_PAGEDOWN   },
    { __STRL("insert")     , KEY_INSERT     },
    { __STRL("delete")     , KEY_DELETE     },
  };

  // Lookup keycode
  for(int i = (int)(sizeof(Translators) / sizeof(KeyNameTranslator)); --i >= 0;)
  {
    const KeyNameTranslator& translator = Translators[i];
    if (translator.KeyNameLength == (int)value.length())
      if (strncmp(value.c_str(), translator.KeyName, translator.KeyNameLength) == 0)
      {
        code = translator.KeyCode;
        return true;
      }
  }

  LOG(LogWarning) << "Unknown key: " << value;
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

  LOG(LogWarning) << "Unknown pad item: " << value;
  return false;
}



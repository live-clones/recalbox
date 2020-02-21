//
// Created by bkg2k on 21/12/2019.
//

#include <utils/Xml.h>
#include <utils/Log.h>
#include <cstring>
#include <utils/Files.h>
#include "PadConfiguration.h"

PadConfiguration::PadConfiguration(const Configuration& configuration)
  : mConfiguration(configuration),
    mPads(),
    mReady(false)
{
  Load();
}

void PadConfiguration::Load()
{
  XmlDocument padXml;
  XmlResult result = padXml.load_string(Files::LoadFile(Path(sInputFile)).c_str());
  if (!result)
  {
    LOG(LogError) << "Could not parse " << sInputFile << " file!";
    return;
  }

  for(PadAllItemConfiguration& pad : mPads)
    for(PadItemConfiguration& item : pad.Items)
      item.Id = -1; // Invalidate every id

  XmlNode inputs = padXml.child("inputList");
  for (const XmlNode padNode : inputs.children())
  {
    const char* padName = padNode.attribute("deviceName").as_string();
    const char* padGuid = padNode.attribute("deviceGUID").as_string();

    // Check
    if ((padName == nullptr) || (padGuid == nullptr)) continue;

    // Lookup every configured pad
    for(int i = Pad2Keyb::MaxPadSupported; --i >= 0; )
      if (mConfiguration.Valid(i))
        if (strcmp(padName, mConfiguration.PadName[i]) == 0)
          if (strcmp(padGuid, mConfiguration.PadGUID[i]) == 0)
          {
            PadAllItemConfiguration& pad = mPads[i];

            static const struct ItemNameTranslator
            {
              const char* XmlName;
              PadItems Item;
            }
            Translators[] =
            {
              { "up"           , PadItems::Up     },
              { "right"        , PadItems::Right  },
              { "down"         , PadItems::Down   },
              { "left"         , PadItems::Left   },
              { "a"            , PadItems::A      },
              { "b"            , PadItems::B      },
              { "x"            , PadItems::X      },
              { "y"            , PadItems::Y      },
              { "pagedown"     , PadItems::L1     },
              { "pageup"       , PadItems::R1     },
              { "l1"           , PadItems::L1     },
              { "r1"           , PadItems::R1     },
              { "l2"           , PadItems::L2     },
              { "r2"           , PadItems::R2     },
              { "l3"           , PadItems::L3     },
              { "r3"           , PadItems::R3     },
              { "select"       , PadItems::Select },
              { "start"        , PadItems::Start  },
              { "hotkey"       , PadItems::Hotkey },
              { "joystick1left", PadItems::J1Left },
              { "joystick1up"  , PadItems::J1Up   },
              { "joystick2left", PadItems::J2Left },
              { "joystick2up"  , PadItems::J2Up   },
            };

            for (const XmlNode ItemNode : padNode.children())
            {
              const char* name  = ItemNode.attribute("name").as_string();
              const char* type  = ItemNode.attribute("type").as_string();
              const char* id    = ItemNode.attribute("id").as_string();
              const char* value = ItemNode.attribute("value").as_string();
              const char* code  = ItemNode.attribute("code").as_string();

              // Check
              if ((name == nullptr) || (type == nullptr) || (id == nullptr) || (value == nullptr) || (code == nullptr))
                continue;

              // Lookup item name
              for (int j = (int) (sizeof(Translators) / sizeof(ItemNameTranslator)); --j >= 0;)
              {
                const ItemNameTranslator& translator = Translators[j];
                if (strcmp(translator.XmlName, name) == 0)
                {
                  PadItemConfiguration& item = pad.Items[(int)translator.Item];
                  // Fill item
                  item.Item = translator.Item;
                  if (strcmp(type, "button") == 0) item.Type = PadItemTypes::Button;
                  else if (strcmp(type, "hat") == 0) item.Type = PadItemTypes::Hat;
                  else if (strcmp(type, "axis") == 0) item.Type = PadItemTypes::Axis;
                  Strings::ToInt(id, item.Id);
                  Strings::ToInt(value, item.Value);
                  Strings::ToInt(code, item.Code);
                  break;
                }
              }
            }
            // Assign J1/J2 down/right
            pad.Items[(int)PadItems::J1Right] = pad.Items[(int)PadItems::J1Left];
            pad.Items[(int)PadItems::J1Down]  = pad.Items[(int)PadItems::J1Up];
            pad.Items[(int)PadItems::J2Right] = pad.Items[(int)PadItems::J2Left];
            pad.Items[(int)PadItems::J2Down]  = pad.Items[(int)PadItems::J2Up];
            pad.Items[(int)PadItems::J1Right].Value = -pad.Items[(int)PadItems::J1Right].Value;
            pad.Items[(int)PadItems::J1Down] .Value = -pad.Items[(int)PadItems::J1Down] .Value;
            pad.Items[(int)PadItems::J2Right].Value = -pad.Items[(int)PadItems::J2Right].Value;
            pad.Items[(int)PadItems::J2Down] .Value = -pad.Items[(int)PadItems::J2Down] .Value;
            pad.Items[(int)PadItems::J1Right].Item  = PadItems::J1Right;
            pad.Items[(int)PadItems::J1Down] .Item  = PadItems::J1Down;
            pad.Items[(int)PadItems::J2Right].Item  = PadItems::J2Right;
            pad.Items[(int)PadItems::J2Down] .Item  = PadItems::J2Down;
            break;
          }
  }

  mReady = true;
}



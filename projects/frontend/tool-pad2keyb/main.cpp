//
// Created by bkg2k on 21/12/2019.
//

#include <cstring>
#include <bits/types/siginfo_t.h>
#include <csignal>

#include <utils/Log.h>

#define __STRL(x) x, sizeof(x)-1

#include <padtokeyboard/Configuration.h>
#include <padtokeyboard/keyboard/VirtualKeyboard.h>
#include <padtokeyboard/pad/MappingConfiguration.h>
#include <padtokeyboard/pad/Pad.h>
#include <padtokeyboard/pad/PadConfiguration.h>

Configuration ParseArguments(int argc, char *argv[])
{
  Configuration result {};

  const struct ArgumentDescriptor
  {
    const char* Switch;
    int SwitchLength;
    const char** TargetValue;
    bool Numbered;
  }
  Descriptors[] =
  {
    { __STRL("-rom") , &result.RomPath,      false },
    { __STRL("-name"), &result.PadName[0],   true  },
    { __STRL("-guid"), &result.PadGUID[0],   true  },
    { __STRL("-dev") , &result.PadDevice[0], true  },
  };

  for(int index = -1; ++index < argc; )
  {
    const char* arg = argv[index];
    if (strcmp(arg, "--debug") == 0) result.Debug = true;
    else
      for(int i = (int)(sizeof(Descriptors) / sizeof(ArgumentDescriptor)); --i >= 0; )
      {
        const ArgumentDescriptor& descriptor = Descriptors[i];
        if (strncmp(arg, descriptor.Switch, descriptor.SwitchLength) == 0 && (index < argc - 1))
        {
          unsigned int number = 0;
          if (descriptor.Numbered) number = (unsigned int)(arg[descriptor.SwitchLength] - 0x30u);
          if (number < Input::sMaxInputDevices)
            Descriptors[i].TargetValue[number] = argv[++index];
        }
      }
  }

  return result;
}

static Pad* InitializedPad = nullptr;

void SigTermHandler(int signum, siginfo_t *info, void *ptr)
{
  (void)signum;
  (void)info;
  (void)ptr;
  // Kill the pad reader when receiving a SIGTERM
  if (InitializedPad != nullptr)
    InitializedPad->Release();
}

void CatchSigterm()
{
  static struct sigaction signalAction;

  memset(&signalAction, 0, sizeof(signalAction));
  signalAction.sa_sigaction = SigTermHandler;
  signalAction.sa_flags = SA_SIGINFO;

  sigaction(SIGTERM, &signalAction, nullptr);
}

int main(int argc, char *argv[])
{
  // Catch kill request
  CatchSigterm();

  // Keep it simple: Log are saved in emulationstation log file
  Log::open();
  Log::setReportingLevel(LogLevel::LogDebug);
  LOG(LogInfo) << "Running pad2keyboard...";

  // Parse command line arguments: get pad description
  Configuration configuration = ParseArguments(argc, argv);
  if (configuration.Debug) Log::setReportingLevel(LogLevel::LogDebug);
  if (!configuration.Valid())
  {
    LOG(LogError) << "Missing arguments.";
    return 1;
  }

  // Get pad => keyboard mapping - Checked at the very beginning to quit gracefully if no config is found
  MappingConfiguration pad2KeybMapping(Path(configuration.RomPath));
  if (!pad2KeybMapping.Valid())
  {
    LOG(LogInfo) << "No pad2keyb configuration.";
    return 0;
  }
  LOG(LogInfo) << "[Pad2Keyboard] user mapping loaded loaded.";

  // Get pad mapping from /recalbox/share/system/.emulationstation/es_input.cfg
  PadConfiguration padMapping(configuration);
  if (!padMapping.Ready())
  {
    LOG(LogError) << "Cannot load pad configurations.";
    return 1;
  }
  LOG(LogInfo) << "[Pad2Keyboard] pad configuration loaded.";

  // Get pad event reader
  Pad pad(padMapping, configuration);
  if (!pad.Ready())
  {
    LOG(LogError) << "Cannot access pad.";
    return 1;
  }
  LOG(LogInfo) << "[Pad2Keyboard] Pad reader is ready.";

  // Create virtual keyboard device
  VirtualKeyboard keyboard;
  keyboard.Open();
  if (!keyboard.Ready())
  {
    LOG(LogError) << "Cannot create virtual keyboard.";
    return 1;
  }
  LOG(LogInfo) << "[Pad2Keyboard] Virtual keyboard is ready.";

  InitializedPad = &pad; // Give SIGTERM intercepter an access to the Pad reader
  Pad::Event event {};
  while(pad.GetEvent(event))
  {
    LOG(LogWarning) << "Event read! " << (int)event.Item << " - " << (int)event.Pad << (int)event.On;
    VirtualKeyboard::Event keyboardEvent = pad2KeybMapping.Translate(event);
    LOG(LogWarning) << "Translated! " << (int)keyboardEvent.Key << " - " << (int)keyboardEvent.Pressed;
    if (keyboardEvent.Key != 0) // Valid?
    {
      keyboard.Write(keyboardEvent);
      LOG(LogDebug) << "Key: " << keyboardEvent.Key << (keyboardEvent.Pressed ? " Pressed" : " Released");
    }
  }
  InitializedPad = nullptr;

  return 0;
}
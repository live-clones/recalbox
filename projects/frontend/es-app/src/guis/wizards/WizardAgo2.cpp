//
// Created by bkg2k on 11/11/2020.
//

#include "WizardAgo2.h"
#include <RecalboxConf.h>
#include <help/Help.h>

Path WizardAGO2::OnImageRequired(int page)
{
  switch((Pages)page)
  {
    case Pages::Intro: return Path(sImagePath) / sIntroImageFile;
    case Pages::SelectButton: return Path(sImagePath) / sSelectImageFile;
    case Pages::StartButton: return Path(sImagePath) / sStartImageFile;
    case Pages::VolumeButtons: return Path(sImagePath) / sVolumeImageFile;
    case Pages::BrightnessButtons: return Path(sImagePath) / sBrightnessImageFile;
    case Pages::PowerSuspend:
    case Pages::PowerOff: return Path(sImagePath) / sPowerImageFile;
    case Pages::Jack: return Path(sImagePath) / sJackImageFile;
    case Pages::Final: return Path(sImagePath) / sFinalImageFile;
    case Pages::Count: break;
  }

  return Path();
}

std::string WizardAGO2::OnTextRequired(int page)
{
  switch((Pages)page)
  {
    case Pages::Intro: return _("Welcome to RECALBOX for Odroid Go Advance!\nThis little presentation will show you how to use the 6 special buttons available right under the screen.\n\nPress any button to start!");
    case Pages::SelectButton: return  _("The leftmost button is the SELECT button, marked with a 'I', also available on most modern pads.\nBut it is also the HOTKEY button that you can use in conjunction with other buttons in most emulators.\nFor example, you can use HOTKEY+START to quit the current game.\n\nPress the SELECT button.");
    case Pages::StartButton: return  _("Next to the SELECT button is the START button, marked with an 'II'.\nUse it to open the main menu in the Recalbox interface and use it in-game as the regular START button available on most consoles.\n\nPress this START button please.");
    case Pages::VolumeButtons: return  _("Then, there are 2 buttons marked with a 'III' and a 'IV'.\nUse them to raise or lower the volume at any time in the Recalbox interface or in-game.\n\n Press either volume up or down");
    case Pages::BrightnessButtons: return  _("The last two buttons marked 'V' and 'VI' are useful to make your screen darker or brighter.\nNote that the brighter the screen, the more power it consumes!\n\n Press either brightness up or down (V/VI)");
    case Pages::PowerSuspend: return _("Just a few words about the POWER button.\nMake a short press, just like a mouse click, and your console will enter sleep mode. Make another short press and your console will restart instanly! Work in Recalbox interface and in-game!\n\nPress button B to continue.");
    case Pages::PowerOff: return _("If you push the POWER button more than 2 seconds, this will power-off your console. If you do so in-game, Recalbox will close the current emulator gracefully.\nJust in case, holding the POWER button down more than 5s perform an hard power-off.\n\nPress button B to continue.");
    case Pages::Jack: return _("One more thing to know: If you plug in or unplug your headphones in the jack connector, Recalbox will automatically switch the audio output. Convenient.\n\nPress button B, as usual.");
    case Pages::Final: return  _("Now you're ready to start your RETROGAMING experience with Recalbox! Press button B to start... and PLAY AGAIN!");
    case Pages::Count: break;
  }

  return std::string();
}

Wizard::Move WizardAGO2::OnKeyReceived(int page, const InputCompactEvent& event)
{
  if (event.CancelPressed()) return Move::Backward;

  switch((Pages)page)
  {
    case Pages::Intro:
    {
      if (event.AnyButtonPressed()) return Move::Foreward;
      break;
    }
    case Pages::StartButton:
    {
      if (event.StartPressed()) return Move::Foreward;
      break;
    }
    case Pages::SelectButton:
    {
      if (event.SelectPressed()) return Move::Foreward;
      break;
    }
    case Pages::VolumeButtons:
    {
      if (event.VolumeUpPressed() || event.VolumeDownPressed()) return Move::Foreward;
      break;
    }
    case Pages::BrightnessButtons:
    {
      if (event.BrightnessDownPressed() || event.BrightnessUpPressed()) return Move::Foreward;
      break;
    }
    case Pages::PowerSuspend:
    case Pages::PowerOff:
    case Pages::Jack:
    {
      if (event.ValidPressed()) return Move::Foreward;
      break;
    }
    case Pages::Final:
    {
      if (event.ValidPressed())
      {
        RecalboxConf::Instance().SetFirstTimeUse(false);
        return Move::Close;
      }
      break;
    }
    case Pages::Count: break;
  }

  if (event.UpPressed()) ChangeLanguage(true);
  else if (event.DownPressed()) ChangeLanguage(false);

  return Move::None;
}


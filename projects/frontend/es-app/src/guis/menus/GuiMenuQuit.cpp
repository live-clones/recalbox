//
// Created by bkg2k on 12/02/2020.
//

#include <guis/menus/GuiMenuQuit.h>
#include <guis/GuiMsgBox.h>
#include <utils/locale/LocaleHelper.h>
#include <MainRunner.h>


GuiMenuQuit::GuiMenuQuit(WindowManager& window)
  : GuiMenuBase(window, _("QUIT"), this)
{
  // Shutdown
  AddSubMenu(_("SHUTDOWN SYSTEM"), (int)Components::Shutdown);

  // Fast Shutdown
  AddSubMenu(_("FAST SHUTDOWN SYSTEM"), (int)Components::FastShutdown);

  // Reboot
  AddSubMenu(_("RESTART SYSTEM"), (int)Components::Reboot);
}

void GuiMenuQuit::PushQuitGui(WindowManager& window)
{
  window.pushGui(new GuiMenuQuit(window));
}

void GuiMenuQuit::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::Shutdown:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN?"), _("YES"),
                                        [] { MainRunner::RequestQuit(MainRunner::ExitState::Shutdown); }, _("NO"), nullptr));
      break;
    }
    case Components::FastShutdown:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"),
                                        [] { MainRunner::RequestQuit(MainRunner::ExitState::FastShutdown); }, _("NO"), nullptr));
      break;
    }
    case Components::Reboot:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY RESTART?"), _("YES"),
                                        [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), nullptr));
      break;
    }
  }
}

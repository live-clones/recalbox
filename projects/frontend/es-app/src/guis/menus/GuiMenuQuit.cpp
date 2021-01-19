//
// Created by bkg2k on 12/02/2020.
//

#include <guis/menus/GuiMenuQuit.h>
#include <guis/GuiMsgBox.h>
#include <utils/locale/LocaleHelper.h>
#include <MainRunner.h>


GuiMenuQuit::GuiMenuQuit(WindowManager& window)
  : GuiMenuBase(window, _("QUIT"))
{
  // Shutdown
  AddSubMenu(_("SHUTDOWN SYSTEM"), mTheme.menuIconSet.shutdown, std::bind(GuiMenuQuit::Shutdown, this));

  // Fast Shutdown
  AddSubMenu(_("FAST SHUTDOWN SYSTEM"), mTheme.menuIconSet.fastshutdown, std::bind(GuiMenuQuit::FastShutdown, this));

  // Reboot
  AddSubMenu(_("RESTART SYSTEM"), mTheme.menuIconSet.restart, std::bind(GuiMenuQuit::Reboot, this));
}

void GuiMenuQuit::PushQuitGui(WindowManager& window)
{
  window.pushGui(new GuiMenuQuit(window));
}

void GuiMenuQuit::Shutdown(GuiMenuQuit* thiz)
{
  thiz->mWindow.pushGui(new GuiMsgBox(thiz->mWindow, _("REALLY SHUTDOWN?"), _("YES"),
                                [] { MainRunner::RequestQuit(MainRunner::ExitState::Shutdown); }, _("NO"), nullptr));

}

void GuiMenuQuit::FastShutdown(GuiMenuQuit* thiz)
{
  thiz->mWindow.pushGui(new GuiMsgBox(thiz->mWindow, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"),
                                [] { MainRunner::RequestQuit(MainRunner::ExitState::FastShutdown); }, _("NO"), nullptr));
}

void GuiMenuQuit::Reboot(GuiMenuQuit* thiz)
{
  thiz->mWindow.pushGui(new GuiMsgBox(thiz->mWindow, _("REALLY RESTART?"), _("YES"),
                                [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), nullptr));
}

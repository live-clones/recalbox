//
// Created by bkg2k on 07/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuNetplay.h"
#include <guis/MenuMessages.h>
#include <utils/locale/LocaleHelper.h>
#include <systems/SystemManager.h>
#include <components/SwitchComponent.h>
#include <guis/GuiNetPlayEditPasswords.h>
#include <guis/GuiArcadeVirtualKeyboard.h>
#include <guis/GuiHashStart.h>
#include <guis/GuiMsgBox.h>

GuiMenuNetplay::GuiMenuNetplay(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("NETPLAY SETTINGS"), this)
  , mThreadPool(this, "Crc32", false)
  , mSystemManager(systemManager)
  , mTotalGames(0)
  , mRemainingGames(0)
  , mPreviousProgressPercent(0)
  , mOperation(nullptr)
{
  // Netplay Enabled
  mEnabled = AddSwitch(_("NETPLAY"), RecalboxConf::Instance().GetNetplayEnabled(), (int)Components::Enabled, this, _(MENUMESSAGE_NP_ONOFF_HELP_MSG));

  // netplay username
  mLogin = AddEditable(_("NICKNAME"), RecalboxConf::Instance().GetNetplayLogin(), (int)Components::UserName, this, false);

  // netplay port
  mPort = AddEditable(_("PORT"), Strings::ToString(RecalboxConf::Instance().GetNetplayPort()), (int)Components::Port, this, false);

  //mitm
  mMitm = AddList<RecalboxConf::Relay>(_("NETPLAY MITM"), (int)Components::Mitm, this, GetMitmEntries(), _(MENUMESSAGE_NP_RELAY_HELP_MSG));

  // Password edition
  AddSubMenu(_("PREDEFINED PASSWORDS"), (int)Components::Passwords);

  // Hash roms
  AddSubMenu(_("HASH ROMS"), (int)Components::Hash, _(MENUMESSAGE_NP_HASH_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<RecalboxConf::Relay>> GuiMenuNetplay::GetMitmEntries()
{
  std::vector<ListEntry<RecalboxConf::Relay>> list;

  RecalboxConf::Relay currentMitm = RecalboxConf::Instance().GetNetplayRelay();
  list.push_back({ _("NONE"), RecalboxConf::Relay::None, currentMitm == RecalboxConf::Relay::None });
  list.push_back({ _("NEW YORK"), RecalboxConf::Relay::NewYork, currentMitm == RecalboxConf::Relay::NewYork });
  list.push_back({ _("MADRID"), RecalboxConf::Relay::Madrid, currentMitm == RecalboxConf::Relay::Madrid });
  list.push_back({ _("MONTREAL"), RecalboxConf::Relay::Montreal, currentMitm == RecalboxConf::Relay::Montreal });
  list.push_back({ _("SAOPAULO"), RecalboxConf::Relay::Saopaulo, currentMitm == RecalboxConf::Relay::Saopaulo });

  return list;
}

void GuiMenuNetplay::EditableComponentTextChanged(int id, const std::string& text)
{
  switch((Components)id)
  {
    case Components::UserName:
    {
      RecalboxConf::Instance().SetNetplayLogin(text).Save();
      break;
    }
    case Components::Port:
    {
      int p = RecalboxConf::sNetplayDefaultPort;
      Strings::ToInt(text, p);
      if ((unsigned int)p > 65535) p = 65535;
      RecalboxConf::Instance().SetNetplayPort(p).Save();
      break;
    }
    case Components::Enabled:
    case Components::Mitm:
    case Components::Passwords:
    case Components::Hash: break;
  }
}

void GuiMenuNetplay::SubMenuSelected(int id)
{
  if ((Components)id == Components::Passwords) mWindow.pushGui(new GuiNetPlayEditPasswords(mWindow));
  else if ((Components)id == Components::Hash) mWindow.pushGui(new GuiHashStart(mWindow, mSystemManager));
}

void GuiMenuNetplay::OptionListComponentChanged(int id, int index, const RecalboxConf::Relay& value)
{
  (void)index;
  if ((Components)id == Components::Mitm)
    RecalboxConf::Instance().SetNetplayRelay(value).Save();
}

void GuiMenuNetplay::SwitchComponentChanged(int id, bool status)
{
  RecalboxConf::Instance().SetNetplayEnabled(status).Save();
  if ((Components)id == Components::Enabled && status)
  {
    bool needHashRefresh = false;
    for(SystemData* system : mSystemManager.GetAllSystemList())
      if (system->Descriptor().HasNetPlayCores())
        if (system->MasterRoot().HasMissingHashRecursively())
        {
          needHashRefresh = true;
          break;
        }
    if (needHashRefresh)
      mWindow.pushGui(new GuiMsgBox(mWindow, _("Some games are not netplay ready yet. "), _("OK"), [this] { StartHashing(); }));
  }
}

void GuiMenuNetplay::StartHashing()
{
  mWindow.pushGui((new GuiWaitLongExecution<bool, bool>(mWindow, *this))->Execute(true, _("Preparing Games...")));
}

bool GuiMenuNetplay::Execute(GuiWaitLongExecution<bool, bool>& from, const bool& parameter)
{
  (void)from;
  (void)parameter;

  // Just sleep 2s to make the long operation bar readable :)
  Thread::Sleep(2000);

  // Collect non-hashed roms...
  FileData::List list;
  for(SystemData* system : mSystemManager.GetAllSystemList())
    if (system->Descriptor().HasNetPlayCores())
      system->MasterRoot().getMissingHashRecursively(list);
  // Inject into thread pool
  for(FileData* game : list)
    mThreadPool.PushFeed(game);

  mTotalGames = mThreadPool.PendingJobs();
  mRemainingGames = mTotalGames;

  // Run!
  mOperation = &from;
  if (mRemainingGames != 0) mThreadPool.Run(-4, true);
  mWaiter.WaitSignal();
  mOperation = nullptr;

  return true;
}

FileData* GuiMenuNetplay::ThreadPoolRunJob(FileData*& feed)
{
  feed->CalculateHash();
  {
    Mutex::AutoLock locker(mSyncher);
    // Update
    if (--mRemainingGames <= 0)
      mWaiter.Fire();
    else
    {
      int percent = 100 - (mRemainingGames * 100) / mTotalGames;
      if (percent != mPreviousProgressPercent)
      {
        mPreviousProgressPercent = percent;
        mOperation->SetText(_("Preparing Games...").append(Strings::ToString(percent)).append(1, '%'));
      }
    }
  }
  return feed;
}


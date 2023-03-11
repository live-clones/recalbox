#include <systems/SystemManager.h>
#include <RecalboxConf.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuTateVirtualSystem.h"
#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"

GuiMenuTateVirtualSystem::GuiMenuTateVirtualSystem(WindowManager& window)
  : GuiMenuBase(window, _("TATE VIRTUAL SYSTEM"), nullptr)
  , mOriginalTateEnabled(RecalboxConf::Instance().GetCollectionTate())
  , mOriginalRotateSystemView(RecalboxConf::Instance().GetCollectionTateRotateSystemView())
  , mOriginalRotateGames(RecalboxConf::Instance().GetCollectionTateRotateGames())
  , mOriginalRotation(RecalboxConf::Instance().GetCollectionTateRotation())
{
  // Enable virtual system
  mTateEnabled = AddSwitch(_("ENABLE TATE VIRTUAL SYSTEM"), mOriginalTateEnabled, (int)Components::TateEnabled, this);

  // Rotate games
  if(Board::Instance().GetRotationCapabilities().canRotate)
  {
    mRotateGames = AddSwitch(_("ROTATE GAMES"), mOriginalRotateGames, (int)Components::TateRotateGames, this);
    // Rotate system view
    mRotateSystemView = AddSwitch(_("ROTATE SYSTEM VIEW"), mOriginalRotateSystemView, (int)Components::TateRotateSystemView, this);
    if(Board::Instance().GetRotationCapabilities().defaultRotationWhenTate == RotationType::None)
    {
      // Rotation
      mRotation = AddList<int>(_("Rotation"), (int)Components::TateRotation, this, GetRotationEntries());
    }
  }
}

GuiMenuTateVirtualSystem::~GuiMenuTateVirtualSystem()
{
  if ((mTateEnabled->getState() != mOriginalTateEnabled) ||
      (mRotateGames && mRotateGames->getState() != mOriginalRotateGames) ||
      (mRotateSystemView && mRotateSystemView->getState() != mOriginalRotateGames) ||
      (mRotation && mRotation->getSelected() != mOriginalRotation))
    RequestRelaunch();
}

std::vector<GuiMenuBase::ListEntry<int>> GuiMenuTateVirtualSystem::GetRotationEntries()
{
  std::vector<GuiMenuBase::ListEntry<int>> list;
  list.push_back({ "NONE", 1, mOriginalRotation == 0 });
  list.push_back({ "LEFT", 1, mOriginalRotation == 1 });
  list.push_back({ "RIGHT", 3, mOriginalRotation == 3 });

  return list;
}

void GuiMenuTateVirtualSystem::OptionListComponentChanged(int id, int index, const int& value)
{
  (void)index;
  if ((Components)id == Components::TateRotation)
  {
    RecalboxConf::Instance().SetCollectionTateRotation(value).Save();
  }
}

void GuiMenuTateVirtualSystem::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::TateRotateSystemView: RecalboxConf::Instance().SetCollectionTateRotateSystemView(status).Save(); break;
    case Components::TateRotateGames: RecalboxConf::Instance().SetCollectionTateRotateGames(status).Save(); break;
    case Components::TateEnabled: RecalboxConf::Instance().SetCollectionTate(status).Save(); break;
    case Components::TateRotation:break;
  }
}


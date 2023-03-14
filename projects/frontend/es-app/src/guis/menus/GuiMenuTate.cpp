#include <systems/SystemManager.h>
#include <RecalboxConf.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuTate.h"
#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "guis/MenuMessages.h"
#include "recalbox/BootConf.h"

GuiMenuTate::GuiMenuTate(WindowManager& window)
  : GuiMenuBase(window, _("TATE SETTINGS"), nullptr)
  , mOriginalTateEnabled(RecalboxConf::Instance().GetCollectionTate())
  , mOriginalGamesRotation(RotationType::None)
  , mOriginalSystemRotation(BootConf::Instance().GetRotation())
{
  // Enable virtual system
  mTateEnabled = AddSwitch(_("ENABLE TATE VIRTUAL SYSTEM"), mOriginalTateEnabled, (int)Components::TateEnabled, this);

  // Rotate games
  RotationCapability cap = Board::Instance().GetRotationCapabilities();

  if(cap.canRotate)
  {
    if(cap.defaultRotationWhenTate != RotationType::None)
    {
      mOriginalGamesRotation = cap.defaultRotationWhenTate;
      mGamesRotation = AddList<RotationType>(_("GAMES ROTATION"), (int)Components::TateGamesRotation, this, std::vector<GuiMenuBase::ListEntry<RotationType>>(1, { "AUTO", mOriginalGamesRotation, true }));
    }
    else
    {
      mOriginalGamesRotation = RotationUtils::FromUint(RecalboxConf::Instance().GetTateGameRotation());
      // Rotation
      mGamesRotation = AddList<RotationType>(_("GAMES ROTATION"), (int)Components::TateGamesRotation, this, GetRotationEntries(mOriginalGamesRotation));
    }
    // Screen rotation
    mSystemRotation = AddList<RotationType>(_("COMPLETE SYSTEM ROTATION"), (int)Components::TateCompleteSystemRotation, this, GetRotationEntries(mOriginalSystemRotation), _(MENUMESSAGE_TATE_SCREEN_ROTATION));
  }
}

GuiMenuTate::~GuiMenuTate()
{
  if ((mTateEnabled->getState() != mOriginalTateEnabled) ||
      (mSystemRotation && mSystemRotation->getSelected() != mOriginalSystemRotation))
    RequestRelaunch();
}

std::vector<GuiMenuBase::ListEntry<RotationType>> GuiMenuTate::GetRotationEntries(RotationType currentRotation)
{
  std::vector<GuiMenuBase::ListEntry<RotationType>> list;
  list.push_back({ "NONE", RotationType::None, currentRotation == RotationType::None });
  list.push_back({ "LEFT", RotationType::Left, currentRotation == RotationType::Left });
  list.push_back({ "RIGHT", RotationType::Right, currentRotation == RotationType::Right });
  list.push_back({ "UPSIDEDOWN", RotationType::Upsidedown, currentRotation == RotationType::Upsidedown });

  return list;
}

void GuiMenuTate::OptionListComponentChanged(int id, int index, const RotationType& value)
{
  (void)index;
  if ((Components)id == Components::TateGamesRotation)
  {
    RecalboxConf::Instance().SetTateGameRotation((uint)value).Save();
  }
  else if ((Components)id == Components::TateCompleteSystemRotation)
  {
    BootConf::Instance().SetRotation(value).Save();
  }
}

void GuiMenuTate::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::TateEnabled: RecalboxConf::Instance().SetCollectionTate(status).Save(); break;
  }
}


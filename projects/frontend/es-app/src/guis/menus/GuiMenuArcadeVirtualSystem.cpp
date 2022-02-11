#include <systems/SystemManager.h>
#include <RecalboxConf.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuArcadeVirtualSystem.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"

GuiMenuArcadeVirtualSystem::GuiMenuArcadeVirtualSystem(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ARCADE VIRTUAL SYSTEM"), nullptr)
  , mSystemManager(systemManager)
  , mOriginalArcadeOnOff(RecalboxConf::Instance().GetCollectionArcade())
  , mOriginalIncludeNeogeo(RecalboxConf::Instance().GetCollectionArcadeNeogeo())
  , mOriginalHideOriginals(RecalboxConf::Instance().GetCollectionArcadeHide())
  , mOriginalPosition(RecalboxConf::Instance().GetCollectionArcadePosition())
{
  // Enable arcade
  mArcadeOnOff = AddSwitch(_("ENABLE ARCADE VIRTUAL SYSTEM"), mOriginalArcadeOnOff, (int)Components::ArcadeOnOff, this);

  // Include neogeo?
  mIncludeNeoGeo = AddSwitch(_("INCLUDE NEO-GEO"), mOriginalIncludeNeogeo, (int)Components::IncludeNeogeo, this);

  // Hide original systems?
  mHideOriginals = AddSwitch(_("HIDE ORIGINAL SYSTEMS"), mOriginalHideOriginals, (int)Components::HideOriginals, this);

  // Position in list
  mPosition = AddList<int>(_("POSITION"), (int)Components::Position, this, GetPositionEntries());
}

GuiMenuArcadeVirtualSystem::~GuiMenuArcadeVirtualSystem()
{
  if ((mArcadeOnOff->getState() != mOriginalArcadeOnOff) ||
      (mIncludeNeoGeo->getState() != mOriginalIncludeNeogeo) ||
      (mHideOriginals->getState() != mOriginalHideOriginals) ||
      (mPosition->getSelected() != mOriginalPosition))
    RequestRelaunch();
}

std::vector<GuiMenuBase::ListEntry<int>> GuiMenuArcadeVirtualSystem::GetPositionEntries()
{
  std::vector<GuiMenuBase::ListEntry<int>> list;

  const SystemManager::SystemList& systemList = mSystemManager.GetVisibleSystemList();
  for(int i = 0; i < (int)systemList.size(); ++i)
  {
    std::string after = systemList[i]->FullName();
    std::string before = ((i <= 0) ? systemList[systemList.size() - 1]->FullName() : systemList[i - 1]->FullName());
    std::string displayString = _("BETWEEN %1 AND %2");
    Strings::ReplaceAllIn(displayString, "%1", before);
    Strings::ReplaceAllIn(displayString, "%2", after);
    list.push_back({ displayString, i, i == mOriginalPosition });
  }

  return list;
}

void GuiMenuArcadeVirtualSystem::OptionListComponentChanged(int id, int index, const int& value)
{
  (void)index;
  if ((Components)id == Components::Position)
  {
    RecalboxConf::Instance().SetCollectionArcadePosition(value).Save();
  }
}

void GuiMenuArcadeVirtualSystem::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::ArcadeOnOff: RecalboxConf::Instance().SetCollectionArcade(status).Save(); break;
    case Components::IncludeNeogeo: RecalboxConf::Instance().SetCollectionArcadeNeogeo(status).Save(); break;
    case Components::HideOriginals: RecalboxConf::Instance().SetCollectionArcadeHide(status).Save(); break;
    case Components::Position:break;
  }
}


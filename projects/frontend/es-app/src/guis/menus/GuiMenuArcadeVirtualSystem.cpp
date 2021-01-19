#include <systems/SystemManager.h>
#include <RecalboxConf.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuArcadeVirtualSystem.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "Settings.h"

GuiMenuArcadeVirtualSystem::GuiMenuArcadeVirtualSystem(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ARCADE VIRTUAL SYSTEM"))
  , mSystemManager(systemManager)
  , mOriginalArcadeOnOff(RecalboxConf::Instance().GetCollectionArcade())
  , mOriginalIncludeNeogeo(RecalboxConf::Instance().GetCollectionArcadeNeogeo())
  , mOriginalHideOriginals(RecalboxConf::Instance().GetCollectionArcadeHide())
  , mOriginalPosition(RecalboxConf::Instance().GetCollectionPosition())
{
  // Enable arcade
  mArcadeOnOff = AddSwitch(_("ENABLE ARCADE VIRTUAL SYSTEM"), mOriginalArcadeOnOff, SetArcade);

  // Include neogeo?
  mIncludeNeoGeo = AddSwitch(_("INCLUDE NEO-GEO"), mOriginalIncludeNeogeo,SetIncludeNeoGeo);

  // Hide original systems?
  mHideOriginals = AddSwitch(_("HIDE ORIGINAL SYSTEMS"), mOriginalHideOriginals, SetHideOriginals);

  // Position in list
  mPosition = AddList<int>(_("POSITION"), SetPosition, GetPositionEntries());
}

GuiMenuArcadeVirtualSystem::~GuiMenuArcadeVirtualSystem()
{
  if ((mArcadeOnOff->getState() != mOriginalArcadeOnOff) ||
      (mIncludeNeoGeo->getState() != mOriginalIncludeNeogeo) ||
      (mHideOriginals->getState() != mOriginalHideOriginals) ||
      (mPosition->getSelected() != mOriginalPosition))
    RequestReboot();
}

void GuiMenuArcadeVirtualSystem::SetArcade(bool on)
{
  RecalboxConf::Instance().SetCollectionArcade(on).Save();
}

void GuiMenuArcadeVirtualSystem::SetIncludeNeoGeo(bool on)
{
  RecalboxConf::Instance().SetCollectionArcadeNeogeo(on).Save();
}

void GuiMenuArcadeVirtualSystem::SetHideOriginals(bool on)
{
  RecalboxConf::Instance().SetCollectionArcadeHide(on).Save();
}

void GuiMenuArcadeVirtualSystem::SetPosition(const int& position)
{
  RecalboxConf::Instance().SetCollectionPosition(position).Save();
}

std::vector<GuiMenuBase::ListEntry<int>> GuiMenuArcadeVirtualSystem::GetPositionEntries()
{
  std::vector<GuiMenuBase::ListEntry<int>> list;

  const SystemManager::SystemList& systemList = mSystemManager.GetVisibleSystemList();
  for(int i = 0; i < (int)systemList.size(); ++i)
  {
    std::string after = systemList[i]->getFullName();
    std::string before = ((i <= 0) ? systemList[systemList.size() - 1]->getFullName() : systemList[i - 1]->getFullName());
    std::string displayString = _("BETWEEN %1 AND %2");
    Strings::ReplaceAllIn(displayString, "%1", before);
    Strings::ReplaceAllIn(displayString, "%2", after);
    list.push_back({ displayString, i, i == mOriginalPosition });
  }

  return list;
}


#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <systems/SystemData.h>
#include <components/MenuComponent.h>

template<typename T>
class OptionListComponent;
class SwitchComponent;

class GuiMenuArcadeVirtualSystem : public GuiMenuBase
{
  public:
    //! Constructor
    GuiMenuArcadeVirtualSystem(WindowManager& window, SystemManager& systemManager);

    //! Destructor
    ~GuiMenuArcadeVirtualSystem() override;

  private:
    //! SystemManager instance
    SystemManager& mSystemManager;

    //! Arcade ON/OFF
    std::shared_ptr<SwitchComponent>          mArcadeOnOff;
    //! Include neogeo in arcade
    std::shared_ptr<SwitchComponent>          mIncludeNeoGeo;
    //! Hide original systems
    std::shared_ptr<SwitchComponent>          mHideOriginals;
    //! Position of the arcade system
    std::shared_ptr<OptionListComponent<int>> mPosition;

    //! Original Arcade ON/OFF value
    bool mOriginalArcadeOnOff;
    //! Original Include neogeo value
    bool mOriginalIncludeNeogeo;
    //! Original Hide Original value
    bool mOriginalHideOriginals;
    //! Original position
    int  mOriginalPosition;

    //! Set Arcade On/off
    static void SetArcade(bool on);
    //! Set Arcade On/off
    static void SetIncludeNeoGeo(bool on);
    //! Set Arcade On/off
    static void SetHideOriginals(bool on);
    //! Set Arcade On/off
    static void SetPosition(const int& position);

    std::vector<GuiMenuBase::ListEntry<int>> GetPositionEntries();
};

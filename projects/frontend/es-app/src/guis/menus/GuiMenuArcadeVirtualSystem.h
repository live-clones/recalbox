#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <systems/SystemData.h>
#include <components/MenuComponent.h>

template<typename T>
class OptionListComponent;
class SwitchComponent;

class GuiMenuArcadeVirtualSystem : public GuiMenuBase
                                 , private IOptionListComponent<int>
                                 , private ISwitchComponent
{
  public:
    //! Constructor
    GuiMenuArcadeVirtualSystem(WindowManager& window, SystemManager& systemManager);

    //! Destructor
    ~GuiMenuArcadeVirtualSystem() override;

  private:
    enum class Components
    {
      ArcadeOnOff,
      IncludeNeogeo,
      HideOriginals,
      Position,
    };

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


    //! Get position entries
    std::vector<GuiMenuBase::ListEntry<int>> GetPositionEntries();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const int& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};

#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <systems/SystemData.h>
#include <components/MenuComponent.h>

template<typename T>
class OptionListComponent;
class SwitchComponent;

class GuiMenuTateVirtualSystem : public GuiMenuBase
                               , private IOptionListComponent<int>
                               , private ISwitchComponent
{
  public:
    //! Constructor
    GuiMenuTateVirtualSystem(WindowManager& window);

    //! Destructor
    ~GuiMenuTateVirtualSystem() override;

  private:
    enum class Components
    {
      TateEnabled,
      TateRotation,
      TateRotateSystemView,
      TateRotateGames,
    };

    //! Tate system enabled
    std::shared_ptr<SwitchComponent>                    mTateEnabled;
    //! Rotate System view ON/OFF
    std::shared_ptr<SwitchComponent>                    mRotateSystemView;
    //! Rotate Games ON/OFF
    std::shared_ptr<SwitchComponent>                    mRotateGames;
    //! Rotation
    std::shared_ptr<OptionListComponent<int>>           mRotation;

    //! Original enabled value
    int mOriginalTateEnabled;
    //! Original Rotate System view ON/OFF value
    bool mOriginalRotateSystemView;
    //! Original Rotate Games value
    bool mOriginalRotateGames;
    //! Original Rotation value
    int mOriginalRotation;

    //! Get rotations entries
    std::vector<GuiMenuBase::ListEntry<int>> GetRotationEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */
    void OptionListComponentChanged(int id, int index, const int& value) override;

    /*
     * ISwitchComponent implementation
     */
    void SwitchComponentChanged(int id, bool status) override;
};

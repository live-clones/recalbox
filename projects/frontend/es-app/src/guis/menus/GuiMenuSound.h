//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SwitchComponent.h>
#include <components/OptionListComponent.h>

class GuiMenuSound : public GuiMenuBase
                   , private ISliderComponent
                   , private ISwitchComponent
                   , private IOptionListComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSound(WindowManager& window);

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

  private:
    enum class Components
    {
      Volume,
      Music,
      Output,
    };

    //! Volume slider
    std::shared_ptr<SliderComponent> mVolume;
    //! Music on/off
    std::shared_ptr<SwitchComponent> mMusic;
    //! Outputs
    std::shared_ptr<OptionListComponent<std::string>> mOutputList;

    //! Get Output List
    static std::vector<ListEntry<std::string>> GetOutputEntries();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*
     * ISliderComponent implementation
     */

    void SliderMoved(int id, float value) override;
};

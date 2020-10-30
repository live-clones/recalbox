//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/SwitchComponent.h>
#include <components/OptionListComponent.h>

class GuiMenuSound : public Gui
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSound(Window& window);

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Fill in help
     * @param help Help
     * @return true of the help system has been files
     */
    bool getHelpPrompts(Help& help) override;

  private:
    //! Menu
    MenuComponent mMenu;

    //! Volume slider
    std::shared_ptr<SliderComponent> mVolume;
    //! Music on/off
    std::shared_ptr<SwitchComponent> mMusic;
    //! Outputs
    std::shared_ptr<OptionListComponent<std::string>> mOutputList;

    static void SetVolume(float volume);

    static void SetMusicOnOff(bool on);

    static void SetOutput(const std::string& output);
};

//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SwitchComponent.h>
#include <components/OptionListComponent.h>

class GuiMenuSound : public GuiMenuBase
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

  private:
    //! Volume slider
    std::shared_ptr<SliderComponent> mVolume;
    //! Music on/off
    std::shared_ptr<SwitchComponent> mMusic;
    //! Outputs
    std::shared_ptr<OptionListComponent<std::string>> mOutputList;

    /*!
     * @brief Set & save volume
     * @param volume new Volume
     */
    static void SetVolume(float volume);

    /*!
     * @brief Se & save music on/off state
     * @param on new Music on/off state
     */
    static void SetMusicOnOff(bool on);

    /*!
     * @brief Set & save selected audio output
     * @param output new Audio output
     */
    static void SetOutput(const std::string& output);
};

//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <guis/Gui.h>
#include <components/MenuComponent.h>

class GuiMenuBase : public Gui
{
  public:
    //! Constructor
    explicit GuiMenuBase(Window& window, const std::string& title);

    /*!
     * @brief Proces input
     * @param event Inpuit event
     * @return True if the input has been processed
     */
    bool ProcessInput(const InputCompactEvent& event) override;

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

  protected:
    //! Menu
    MenuComponent mMenu;
    //! Menu initialized flag
    bool mMenuInitialized;

    void AddSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help = "");
};

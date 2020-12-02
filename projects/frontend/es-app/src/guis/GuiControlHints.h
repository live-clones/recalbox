//
// Created by bkg2k on 03/05/2020.
//
#pragma once

#include <guis/Gui.h>
#include <utils/os/fs/Path.h>
#include <components/ComponentGrid.h>
#include <components/NinePatchComponent.h>
#include <components/TextComponent.h>

class GuiControlHints : public Gui
{
  private:
    //! Grid component, 4 cells per Help item
    ComponentGrid mGrid;

    //! "Window" mackground
    NinePatchComponent mBackground;

    //! Title
    std::shared_ptr<TextComponent> mTitle;

    //! Rows
    int mRows;
    //! Columns
    int mColumns;

    /*!
     * @brief Notification of an input event
     * @param event Compact event
     * @return Implementation must return true if it consumed the event.
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Called when the component size is changed
     */
    void onSizeChanged() override;

    /*!
     * @brief Fill help list
     * @param help Help list to fill
     * @return True if the method filled the list, false otherwise
     */
    bool getHelpPrompts(Help& help) override;

  public:
    /*!
     * @brief Constructor
     * @param window Main window
     */
    explicit GuiControlHints(WindowManager& window, const Path& p2kPath);
};

//
// Created by bkg2k on 11/11/2020.
//
#pragma once

#include <guis/Gui.h>
#include <utils/os/fs/Path.h>
#include <components/ComponentGrid.h>
#include <components/NinePatchComponent.h>

// Forward declaration
class TextComponent;
class ImageComponent;

class Wizard : public Gui
{
  public:
    enum class Move
    {
        None,
        Backward,
        Foreward,
        Close,
    };

    /*!
     * @brief Constructor
     * @param window Main window
     * @param title Wizard title
     * @param pageCount Total page count
     */
    Wizard(WindowManager& window, const std::string& title, int pageCount);

    /*!
     * @brief Called when a new page is displayed
     * @param page Page to display
     * @return Image to display
     */
    virtual Path OnImageRequired(int page) = 0;

    /*!
     * @brief Called when a new page is displayed
     * @param page Page to display
     * @return Text to display
     */
    virtual std::string OnTextRequired(int page) = 0;

    /*!
     * @brief Called when the help bar is being to be refreshed
     * @param page Page to display
     * @param help Help to fill-in
     */
    virtual void OnHelpRequired(int page, Help& help) = 0;

    /*!
     * @brief Ask the wizard to react to the given input event
     * @param page Current page
     * @param event Input event
     * @return None if the event is of no interest, Forward or Backward if the wizard must move, Close to close the wizard
     */
    virtual Move OnKeyReceived(int page,const InputCompactEvent& event) = 0;

    /*!
     * @brief Force a refresh of the current page
     */
    void UpdatePage();

  private:
    //! "Window" background
    NinePatchComponent mBackground;
    //! Global component grid for easy positioning
    ComponentGrid mGrid;

    //! Title
    std::shared_ptr<TextComponent> mTitle;
    //! Left image
    std::shared_ptr<ImageComponent> mImage;
    //! Right text
    std::shared_ptr<TextComponent> mText;

    //! Page count
    int mPageCount;
    //! Current page index
    int mCurrentPage;

    /*!
     * @brief Process input events
     * @param event Input event
     * @return True if it has been processed
     */
    bool ProcessInput(const InputCompactEvent& event) final;

    /*!
     * @brief Update frame, used to start the first page
     * @param deltaTime Elapsed time since last page
     */
    void Update(int deltaTime) final;

    /*!
     * @brief Fill the help bar
     * @param help Help bar
     * @return True
     */
    bool getHelpPrompts(Help& help) final;

    /*!
     * @brief Set the active page of the wizard
     * @param page new active page index
     */
    void SetPage(int page);
};

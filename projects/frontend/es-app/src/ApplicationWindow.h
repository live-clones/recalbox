//
// Created by bkg2k on 19/11/2019.
//
#pragma once

#include <WindowManager.h>
#include <systems/SystemManager.h>
#include <views/ViewController.h>

class ApplicationWindow: public WindowManager
{
  private:
    //! View controler
    ViewController mViewController;

    //! True when the Application window is being closed ASAP
    bool mClosed;

  protected:
    /*!
     * @brief Update the help system.
     * @return True if the update system has been set, false otherwise
     */
    bool UpdateHelpSystem() override;

  public:
    /*!
     * @brief Constructor
     * @param systemManager Systeme manager instance
     */
    explicit ApplicationWindow(SystemManager& systemManager)
      : mViewController(*this, systemManager)
      , mClosed(false)
    {
    }

    bool ProcessInput(const InputCompactEvent& event) override;

    void Update(int deltaTime) override;

    void Render(Transform4x4f& transform) override;

    /*!
     * @brief Return true if the Application window is required to close
     * @return True if the window must close
     */
    bool Closed() const { return mClosed; }

    /*!
     * @brief Check if the given UI is on top of the screen
     * @return True if the given UI is the first visible
     */
    bool AmIOnTopOfScreen(const Gui* ui) const override
    {
      if (HasGui())
        return WindowManager::AmIOnTopOfScreen(ui);
      else
        return &mViewController.CurrentUi() == ui;
    }

    /*!
     * @brief Switch the view controller to the splash screen
     */
    void GoToQuitScreen()
    {
      deleteAllGui();
      mViewController.goToQuitScreen();
      DoWake(); // Avoid screensaver to run
      RenderAll();
    }
};

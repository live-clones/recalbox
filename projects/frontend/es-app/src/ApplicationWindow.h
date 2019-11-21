//
// Created by bkg2k on 19/11/2019.
//
#pragma once

#include <Window.h>
#include <systems/SystemManager.h>
#include <views/ViewController.h>

class ApplicationWindow: public Window
{
  private:
    ViewController mViewController;

    bool mClosed;

  public:
    explicit ApplicationWindow(SystemManager& systemManager)
      : mViewController(*this, systemManager),
        mClosed(false)
    {
    }

    void ProcessInput(const InputCompactEvent& event) override;

    void Update(int deltaTime) override;

    void RenderAllGraphics();

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
        return Window::AmIOnTopOfScreen(ui);
      else
        return mViewController.CurrentUi() == ui;
    }
};

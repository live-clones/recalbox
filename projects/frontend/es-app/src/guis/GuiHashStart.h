//
// Created by xizor on 01/06/18.
//
#pragma once

#include "guis/Gui.h"
#include "systems/SystemData.h"
#include <utils/os/system/Mutex.h>
#include <utils/os/system/Thread.h>
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"

template<typename T>
class OptionListComponent;

class GuiHashStart : public Gui, private Thread
{
  private:
    //! UI State
    enum class State
    {
      Wait,    //!< Waiting for user input
      Hashing, //!< Running hash on selected systems
      Exit,    //!< Close this UI
    };

    //! SystemManager instance
    SystemManager& mSystemManager;

    //! Busy animation
    BusyComponent mBusyAnim;
    //! Selected systems
    std::shared_ptr< OptionListComponent<SystemData*> > mSystems;
    //! Filters
    std::shared_ptr< OptionListComponent<std::string> > mFilter;
    //! Menu
    MenuComponent mMenu;
    //! GUI Global state
    State mState;
    //! Protext mBysuAnim component access from both main & hash threads
    Mutex mMutex;

    /*
     * Thread implementation
     */

    /*!
     * @brief Main hash thread method
     */
    void Run() override;

  public:
    /*!
     * @brief Constructor
     * @param window main ui window
     */
    explicit GuiHashStart(Window& window, SystemManager& systemManager);

    /*!
     * @brief Destructor
     */
    ~GuiHashStart() override { Thread::Stop(); }

    /*!
     * @brief Process input event
     * @param event Input event
     * @return True if the method has processed the inpur event
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Get the help system
     * @return True
     */
    bool getHelpPrompts(Help& help) override;

    /*!
     * @brief Update method, called periodically
     * @param deltaTime Elapsed milliseconds since the last call
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Render UI
     * @param parentTrans Parent transformation
     */
    void Render(const Transform4x4f &parentTrans) override;
};

//
// Created by xizor on 01/06/18.
//
#pragma once

#include "guis/Gui.h"
#include "systems/SystemData.h"
#include <utils/os/system/ThreadPool.h>
#include <utils/os/system/Mutex.h>
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"

template<typename T>
class OptionListComponent;

class GuiHashStart : public Gui, private IThreadPoolWorkerInterface<FileData*, FileData*>
{
  private:
    //! UI State
    enum class State
    {
      Wait,      //!< Waiting for user input
      Hashing,   //!< Running hash on selected systems
      Cancelled, //!< Cancelled by the user, should close ASAP
      Exit,      //!< Close this UI
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
    volatile State mState;
    //! Protext mBysuAnim component access from both main & hash threads
    Mutex mMutex;
    //! Thread pool for // CRC computations
    ThreadPool<FileData*, FileData*> mThreadPool;
    //! Total games
    int mTotalGames;
    //! Remaining games
    int mRemaininglGames;
    //! Output text
    std::string mSummaryText;

    /*!
     * @brief Prepare files and start Crc computations
     */
    void Start();

    /*!
     * @brief Close the Gui and display summary
     */
    void Quit();

    /*
     * Thread implementation
     */

    /*!
     * @brief The main runner. Implement here the task to process a feed object
     * @param feed Feed object to process
     * @return Result Object
     */
    FileData* ThreadPoolRunJob(FileData*& feed) override;

  public:
    /*!
     * @brief Constructor
     * @param window main ui window
     */
    explicit GuiHashStart(WindowManager& window, SystemManager& systemManager);

    /*!
     * @brief Destructor
     */
    ~GuiHashStart() override
    {
      if (mState == State::Hashing)
      {
        mState = State::Cancelled;
        mThreadPool.WaitForCompletion();
      }
    }

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

//
// Created by bkg2k on 07/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <guis/IGuiArcadeVirtualKeyboardInterface.h>
#include <guis/GuiWaitLongExecution.h>
#include <components/IEditableComponent.h>
#include <games/FileData.h>
#include <utils/os/system/ThreadPool.h>
#include <utils/os/system/IThreadPoolWorkerInterface.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuNetplay : public GuiMenuBase
                     , private IOptionListComponent<RecalboxConf::Relay>
                     , private ISwitchComponent
                     , private IGuiMenuBase
                     , private IEditableComponent
                     , private ILongExecution<bool, bool>
                     , private IThreadPoolWorkerInterface<FileData*, FileData*>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuNetplay(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
      Enabled,
      UserName,
      Port,
      Mitm,
      Passwords,
      Hash,
    };

    //! Thread pool for // CRC computations
    ThreadPool<FileData*, FileData*> mThreadPool;
    //! Event
    Signal mWaiter;
    //! Syncher
    Mutex mSyncher;

    //! System manager
    SystemManager& mSystemManager;

    //! Enabled
    std::shared_ptr<SwitchComponent> mEnabled;
    //! Login
    std::shared_ptr<EditableComponent> mLogin;
    //! Port
    std::shared_ptr<EditableComponent> mPort;
    //! Mitm
    std::shared_ptr<OptionListComponent<RecalboxConf::Relay>> mMitm;

    //! Total games
    int mTotalGames;
    //! Remaining games
    int mRemainingGames;
    //! Sored progress
    int mPreviousProgressPercent;
    //! Reference to long operation object
    GuiWaitLongExecution<bool, bool>* mOperation;

    //! Get O/C List
    static std::vector<ListEntry<RecalboxConf::Relay>> GetMitmEntries();

    /*
     * IEditableComponent implementation
     */

    void EditableComponentTextChanged(int id, const std::string& text) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent implementation
     */

    void OptionListComponentChanged(int id, int index, const RecalboxConf::Relay& value) override;

    /*
     * ISWitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*
     * ILongExecution implementation
     */

    /*!
     * @brief Execture network operation
     * @param parameter Network operation required to execute
     * @return True if the operation was successful
     */
    bool Execute(GuiWaitLongExecution<bool, bool>& from, const bool& parameter) final;

    /*!
     * @brief Receive the status of network operations
     * @param parameter original input parameter
     * @param result Result state
     */
    void Completed(const bool& parameter, const bool& result) final { (void)parameter; (void)result; }

    /*
     * Thread implementation
     */

    /*!
     * @brief The main runner. Implement here the task to process a feed object
     * @param feed Feed object to process
     * @return Result Object
     */
    FileData* ThreadPoolRunJob(FileData*& feed) override;

    /*
     * Hash
     */
    void StartHashing();
};




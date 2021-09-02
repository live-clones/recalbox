//
// Created by bkg2k on 02/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <WindowManager.h>
#include "SystemManager.h"

class GameRunner : public StaticLifeCycleControler<GameRunner>
{
  public:
    /*!
     * @brief Constructor
     * @param window Window manager
     * @param systemManager System manager
     */
    GameRunner(WindowManager& window, SystemManager& systemManager)
      : StaticLifeCycleControler<GameRunner>("GameRunner")
      , mWindowManager(window)
      , mSystemManager(systemManager)
    {
    }

    /*!
     * @brief Run game
     * @param game Game to run
     * @param emulator Emulator data
     * @param netplay Netplay data
     * @return True if the game ran fine, false otherwise
     */
    bool RunGame(FileData& game, const EmulatorData& emulator, const NetPlayData& netplay);

    /*!
     * @brief INitialize demo launching
     * @return Controller configuration string
     */
    std::string demoInitialize();

    /*!
     * @brief Finalize demo run
     */
    void demoFinalize();

    /*!
     * @brief Run a game demo
     * @param game Game to run
     * @param emulator Emulator data
     * @param duration Duraction in second
     * @param infoscreenduration Info screen duration in second
     * @param controlersConfig Controller configuration string
     * @return True if the demo exited on user request
     */
    bool DemoRunGame(const FileData& game, const EmulatorData& emulator, int duration, int infoscreenduration, const std::string& controlersConfig);


    /*!
     * @brief Instruct threads that a game is currently running
     * @return True if a game is currently running, false otherwise
     */
    static bool IsGameRunning() { return sGameIsRunning; }

  private:
    //! CPU Governance file
    static constexpr const char* sGovernanceFile = "/recalbox/share_init/system/configs/.governances.cfg";

    //! Window manager reference
    WindowManager& mWindowManager;
    //! System manager reference
    SystemManager& mSystemManager;

    //! Game running flag
    static bool sGameIsRunning;

    /*!
    * @brief Automatic Game running flag management
    */
    class GameRunning
    {
      public:
        //! Constructor - Set the flag
        GameRunning() { sGameIsRunning = true; }
        //! Destructor - Reset the flag
        ~GameRunning() { sGameIsRunning = false; }
    };

    class ThreadRunner : private Thread
    {
      public:
        /*!
         * @brief Constructor
         * @param runner Runner to stop when the game is over
         * @param command Command to execute
         * @param debug Debug flag
         */
        ThreadRunner(Sdl2Runner& runner, const std::string& command, bool debug)
          : mRunner(runner)
          , mCommand(command)
          , mDebug(debug)
          , mExitCode(0)
        {
          Thread::Start("GameRun");
        }

        //! Get exit code
        int ExitCode() const { return mExitCode; }

      private:
        //! Runner to stop when the game is over
        Sdl2Runner& mRunner;
        //! Command to execute
        std::string mCommand;
        //! Debug flag
        bool        mDebug;
        //! Exit code
        int         mExitCode;

        /*
         * Thread implementation
         */

        // Run the thread
        void Run() override
        {
          mExitCode = WEXITSTATUS(GameRunner::Run(mCommand, mDebug));
          mRunner.Stop();
        }
    };

    /*!
     * @brief Run system command and capture output
     * @param cmd_utf8 Command to execute
     * @param debug log output?
     * @return Return code
     */
    static int Run(const std::string& cmd_utf8, bool debug);

    /*!
     * @brief Get energy governance for a particular core/emulator
     * @param core Core/Emulateur
     * @return Governance
     */
    static IBoardInterface::CPUGovernance GetGovernance(const std::string& core);

    /*!
     * @brief Get netplay option regarding the given netplay object
     * @param netplay Netplay object
     * @return Netplay option string
     */
    static std::string NetplayOption(const FileData& game, const NetPlayData& netplay);
};




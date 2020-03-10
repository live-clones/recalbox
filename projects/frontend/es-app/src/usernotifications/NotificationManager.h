//
// Created by Bkg2k on 10/03/2020.
//
#pragma once

#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <systems/SystemData.h>

enum class Notification
{
    None,
    Start,
    Stop,
    Shutdown,
    Reboot,
    Relaunch,
    Quit,
    SystemBrowsing,
    GamelistBrowsing,
    RunGame,
    RunDemo,
    EndGame,
    EndDemo,
    Sleep,
    WakeUp,
    ThemeChanged,
    ConfigurationChanged,
};

class NotificationManager : public StaticLifeCycleControler<NotificationManager>
{
  private:
  /*!
   * @brief Struture to hold a bag of parameters
   */
    struct ParamBag
    {
      //! Action parameters
      std::string mActionParameters;
      //! System
      const SystemData* mSystemData;
      // Game
      const FileData* mFileData;
      // Action
      Notification mAction;

      /*!
       * @brief Default constructor
       */
      ParamBag()
        : mActionParameters(""),
          mSystemData(nullptr),
          mFileData(nullptr),
          mAction(Notification::None)
      {
      }

      /*!
       * @brief Full constructor
       * @param systemData System
       * @param fileData Game
       * @param action Action
       * @param actionParameters Optional action parameters
       */
      ParamBag(const SystemData* systemData, const FileData* fileData, Notification action, const std::string& actionParameters)
        : mActionParameters(actionParameters),
          mSystemData(systemData),
          mFileData(fileData),
          mAction(action)
      {
      }

      /*!
       * @brief Inequality operator
       * @param compareTo Structure to compare against
       * @return True if at least one field is not equal
       */
      bool operator != (const ParamBag& compareTo)
      {
        return ((mAction != compareTo.mAction) ||
                (mSystemData != compareTo.mSystemData) ||
                (mFileData != compareTo.mFileData) ||
                (mActionParameters != compareTo.mActionParameters));
      }
    };

    //! Script folder
    static constexpr const char* sScriptPath = "/recalbox/share/userscripts";

    //! Status file
    static Path sStatusFilePath;

    //! All available scripts
    Path::PathList mScriptList;

    //! Previous data
    ParamBag mPreviousParamBag;

    /*!
     * @brief Convert an Action into a string
     * @param action Action to convert
     * @return Converted string
     */
    static const char* ActionToString(Notification action);

    /*!
     * @brief Convert a string into an Action
     * @param action String to convert
     * @return Converted Action (Action::None if the conversion fails)
     */
    static Notification ActionFromString(const std::string& action);

    /*!
     * @brief Load all available scripts
     */
    void LoadScriptList();

    /*!
     * @brief Get a filtered list from all available list
     * @param action Action to filter
     * @return Filtered list
     */
    Path::PathList FilteredScriptList(Notification action);

    /*!
     * @brief Update EmulationStation status file
     * @param system Target System (may be null)
     * @param game Target game (may be null)
     * @param play True if the target game is going to be launched
     * @param demo True if the target game is going ot be launched as demo
     */
    void Notify(const SystemData* system, const FileData* game, Notification action, const std::string& actionParameters);

    /*!
     * @brief Run all script associated to the given action
     * @param action Action to filter scripts with
     * @param param Optional action parameter
     */
    void RunScripts(Notification action, const std::string& param);

  public:

    /*!
     * @brief Default constructor
     */
    NotificationManager();

    /*!
     * @brief Update EmulationStation status file with game information
     * @param game Target game
     * @param action Action to notify
     */
    void Notify(const FileData& game, Notification action) { Notify(game.getSystem(), &game, action, game.getPath().ToString()); }

    /*!
     * @brief Update EmulationStation status file with system information
     * @param system Target system
     * @param action Action to notify
     */
    void Notify(const SystemData& system, Notification action) { Notify(&system, nullptr, action, system.getFullName()); }

    /*!
     * @brief
     * @param action Action to notify
     * @param actionParameters Optional action parameters
     */
    void Notify(Notification action, const std::string& actionParameters) { Notify(nullptr, nullptr, action, actionParameters); }

    /*!
     * @brief
     * @param action Action to notify
     */
    void Notify(Notification action) { Notify(nullptr, nullptr, action, std::string()); }
};

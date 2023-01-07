//
// Created by digitalLumberjack on 17/06/2022.
//
#pragma once

#include <utils/IniFile.h>
#include <utils/Strings.h>

/*!
* Case management
*
*/
class Case
{
  public:
    enum class CaseModel
    {
      // Automatic
      GPiV1,
      GPiV2,
      GPiV3,
      GPi2,
      GPi2W,
      Nuxii,
      PiBoy,
      // Retroflag auto
      Nespi4Case,
      // Retroflag Manual
      Nespi4CaseManual,
      SuperPi4Case,
      NespiCasePlus,
      PiStation,
      SuperPiCase,
      MegaPiCase,
      ArgonOne,
      None,
    };

    /*!
     * @brief Return the current model enum of this case
     * @return the current model enum of this case
     */
    CaseModel Model() const { return mModel; }

    /*!
     * @brief Some cases are automatically detected by the system
     * @return true if the case is automatically detexted by the system
     */
    bool Automatic() const { return mAutomatic; }

    /*!
     * @brief Some cases have an on/off button with a state
     * @return true if the case allows shutting down recalbox from the menu
     */
    bool CanShutdownFromMenu() const { return mShutdownSupported; }

    /*!
     * @brief Case name should be human readable
     * @return a nice and hamun readable name
     */
    const std::string& DisplayName() const { return mDisplayName; }

    /*!
     * @brief The case name in Recalbox system (used in config files and so)
     * @return the short name of the case
     */
    const std::string& ShortName() const { return mShortName; }

    /*!
     * @brief Install the case on the system. If the case is None, it will uninstall cases.
     */
    bool Install() const;

    /*!
     * @brief Uninstall the case
     */
    bool Uninstall() const;

    /*!
     * @brief Returns the install message
     * @return the installe message
     */
    std::string GetInstallMessage() const { return mInstallMessage; }

    /*!
     * @brief Get the case from short name
     * @param the short name of the case
     * @return the case or a None case if not found
     */
    static Case FromShortName(const std::string& value);

    /*!
     * @brief Factory constructor by model
     * @param the model id
     * @return the case instance of the model
     */
    static Case Create(CaseModel model);

    /*!
     * @brief Returns the installed case (auto or manual)
     * @return the installed case
     */
    static Case CurrentCase();

    /*!
     * @brief Each board can support its own cases
     * @return supported cases depending on the current board
     */
    static std::vector<Case> SupportedManualCases();

  private:
    /*!
     * @brief Private constructor
     * @param model Case model
     * @param automatic Detectyed automatically?
     * @param displayName Displayable name
     * @param shortName Internal name
     */
    Case(CaseModel model, bool automatic, bool shutdownSupported, const std::string& displayName, const std::string& shortName, const std::string& installMessage)
      : mDisplayName(displayName)
      , mShortName(shortName)
      , mInstallMessage(installMessage)
      , mModel(model)
      , mAutomatic(automatic)
      , mShutdownSupported(shutdownSupported)
     {}

    static bool SetCaseInBoot(const std::string& theCase);
    const std::string mDisplayName;
    const std::string mShortName;
    const std::string mInstallMessage;
    const enum CaseModel mModel;
    const bool mAutomatic;
    const bool mShutdownSupported;
};

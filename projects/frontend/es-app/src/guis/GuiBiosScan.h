//
// Created by bkg2k on 31/12/2019.
//
#pragma once

#include <guis/Gui.h>
#include <bios/Bios.h>
#include <bios/BiosList.h>
#include <bios/IBiosScanReporting.h>
#include <components/TextListComponent.h>
#include <components/BusyComponent.h>
#include <systems/SystemManager.h>

class GuiBiosScan : public Gui, public IBiosScanReporting
{
  private:
    //! Text color index
    static constexpr signed char sColorIndexNormal = 0;
    //! Half Text color index
    static constexpr signed char sColorIndexHalf = 1;
    //! Transparent Red
    static constexpr signed char sColorIndexRed = 2;
    //! Transparent Yellow
    static constexpr signed char sColorIndexYellow = 3;
    //! Transparent Green
    static constexpr signed char sColorIndexGreen = 4;

    //! Small structure holding the context of every item
    struct ListContext
    {
      const BiosList* mBiosList;
      const Bios* mBios;
      ListContext()
        : mBiosList(nullptr),
          mBios(nullptr)
      {
      }
      ListContext(const BiosList* bioslist, const Bios* bios)
        : mBiosList(bioslist),
          mBios(bios)
      {
      }
    };

    //! Ubuntu condensed
    std::shared_ptr<Font> mBoldCondensed;

    //! "Window" background
    NinePatchComponent mBackground;
    //! Global component grid for easy positioning
    ComponentGrid mGrid;

    //! Text title
    std::shared_ptr<TextComponent> mTitle;
    //! Header
    std::shared_ptr<TextComponent> mHeader;
    //! Footer
    std::shared_ptr<TextComponent> mFooter;
    //! Bios list
    std::shared_ptr<TextListComponent<ListContext>> mList;
    //! Button grid
    std::shared_ptr<ComponentGrid> mButtonGrid;
    //! Button close
    std::shared_ptr<ButtonComponent> mButtonClose;

    //! Detail: System label
    std::shared_ptr<TextComponent> mDetailSystemLabel;
    //! Detail: System name
    std::shared_ptr<TextComponent> mDetailSystemValue;
    //! Detail: Core label
    std::shared_ptr<TextComponent> mDetailCoreLabel;
    //! Detail: Core names
    std::shared_ptr<TextComponent> mDetailCoreValue;
    //! Detail: Path label
    std::shared_ptr<TextComponent> mDetailPathLabel;
    //! Detail: Path value
    std::shared_ptr<TextComponent> mDetailPathValue;
    //! Detail: Mandatory label
    std::shared_ptr<TextComponent> mDetailMandatoryLabel;
    //! Detail: Mandatory yes/no
    std::shared_ptr<TextComponent> mDetailMandatoryValue;
    //! Detail: Hash Must Match label
    std::shared_ptr<TextComponent> mDetailHashMustMatchLabel;
    //! Detail: Hash Must Match yes/no
    std::shared_ptr<TextComponent> mDetailHashMustMatchValue;
    //! Detail: File found label
    std::shared_ptr<TextComponent> mDetailFileFoundLabel;
    //! Detail: File found status
    std::shared_ptr<TextComponent> mDetailFileFoundValue;
    //! Detail: Hash is matching label
    std::shared_ptr<TextComponent> mDetailHashIsMatchingLabel;
    //! Detail: Hash is matching status
    std::shared_ptr<TextComponent> mDetailHashIsMatchingValue;
    //! Detail: Free text 1 label (core list)
    std::shared_ptr<TextComponent> mDetailText1Label;
    //! Detail: Free text 1 value container
    std::shared_ptr<ScrollableContainer> mDetailText1ValueContainer;
    //! Detail: Free text 1 value (remark / core list)
    std::shared_ptr<TextComponent> mDetailText1Value;
    //! Detail: Free text 2 label (note / core list 2)
    std::shared_ptr<TextComponent> mDetailText2Label;
    //! Detail: Free text 2 value container
    std::shared_ptr<ScrollableContainer> mDetailText2ValueContainer;
    //! Detail: Free text 2 value (note / core list 2)
    std::shared_ptr<TextComponent> mDetailText2Value;

    //! Detail: Bios OK text
    std::shared_ptr<TextComponent> mDetailBiosOkLabel;
    //! Detail: Bios OK value
    std::shared_ptr<TextComponent> mDetailBiosOkValue;
    //! Detail: Bios unsafe text
    std::shared_ptr<TextComponent> mDetailBiosUnsafeLabel;
    //! Detail: Bios unsafe value
    std::shared_ptr<TextComponent> mDetailBiosUnsafeValue;
    //! Detail: Bios KO text
    std::shared_ptr<TextComponent> mDetailBiosKOLabel;
    //! Detail: Bios KO value
    std::shared_ptr<TextComponent> mDetailBiosKOValue;
    //! Detail: Bios Not found label
    std::shared_ptr<TextComponent> mDetailBiosNotFoundLabel;
    //! Detail: Bios Not found value
    std::shared_ptr<TextComponent> mDetailBiosNotFoundValue;
    //! Detail: Bios Matching text
    std::shared_ptr<TextComponent> mDetailBiosMatchingLabel;
    //! Detail: Bios matching value
    std::shared_ptr<TextComponent> mDetailBiosMatchingValue;
    //! Detail: Bios not matching text
    std::shared_ptr<TextComponent> mDetailBiosNotMatchingLabel;
    //! Detail: Bios not matching value
    std::shared_ptr<TextComponent> mDetailBiosNotMatchingValue;

    //! Detail: Image
    std::shared_ptr<ImageComponent> mDetailStatusImage;

    //! Busy animation while scanning
    BusyComponent mBusyAnim;

    //! System Manager instance
    SystemManager& mSystemManager;

    //! Scan is running?
    bool mScanInProgress;

    //! My Systems (false) / All Systems (true)
    bool mShowAllSystems;

    /*!
     * @brief Start (re)scanning bios files
     */
    void StartRescan();

    /*!
     * @brief Update visuial bios list
     */
    void UpdateBiosList();

    /*!
     * @brief Check if at least one system in systemNames is a shown system (with valid roms)
     * @param systemNames systemNames, comma separated
     * @return True if at least one system is a shown system
     */
    bool IsSystemShown(const std::string& systemNames);

    /*!
     * @brief Update bios details of currently selected bios
     */
    void UpdateBiosDetail();

    /*!
     * @brief Called when sizing window
     */
    void onSizeChanged() override;

    /*!
     * @brief Override rendering to draw custom background
     * @param parentTrans parent transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

    /*!
     * @brief Update
     * @param deltaTime Elapsed time since last update
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Process input events
     * @param event Input event
     * @return True if the method consumed the event
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Set help prompts
     * @param help Help structure to fill in
     * @return True
     */
    bool getHelpPrompts(Help& help) override;

    /*!
     * @brief Extract unique core list from the given BiosList
     * @param biosList Source bios list
     * @return Core list, comma separated
     */
    static std::string GetUniqueCoreList(const BiosList& biosList);

    /*
     * IBiosScanReporting implementation
     */

    /*!
     * @brief Report a new Bios has been scanned
     * @param bios Newly scanned bios
     */
    void ScanProgress(const Bios& bios) override;

    /*!
     * @brief Report the bios scan is complete
     */
    void ScanComplete() override;

  public:
    /*!
     * @brief Constructor
     * @param window Main window
     */
    explicit GuiBiosScan(WindowManager& window, SystemManager& systemManager);
};
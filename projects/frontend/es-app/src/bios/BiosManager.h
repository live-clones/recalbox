//
// Created by bkg2k on 28/12/2019.
//
#pragma once

#include <vector>
#include <utils/os/system/Thread.h>
#ifndef PURE_BIOS_ONLY
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#endif
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include "BiosList.h"
#include "IBiosScanReporting.h"

class BiosManager : public StaticLifeCycleControler<BiosManager>
                  , private Thread
#ifndef PURE_BIOS_ONLY
                  , public ISynchronousEvent
#endif
{
  private:
    //! Path to bios.xml file
    static constexpr const char* sBiosFilePath = "system/.emulationstation/es_bios.xml";
    //! Path to report file
    static constexpr const char* sReportPath = "bios/missing_bios_report.txt";

    //! Bios list per system
    std::vector<BiosList> mSystemBiosList;
    #ifndef PURE_BIOS_ONLY
    //! Sync'ed event sender
    SyncronousEvent mSender;
    #endif
    //! Current scan's reporting interface (also flag for an already running scan)
    IBiosScanReporting* mReporting;

    /*
     * Thread implementation
     */

    /*!
     * Threaded scan implementation
     */
    void Run() override;

    #ifndef PURE_BIOS_ONLY
    /*
     * ISynchronousInterface
     */

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;
    #endif

  public:
    /*!
     * @brief Default constructor
     */
    BiosManager();

    /*!
     * @brief Default destructor
     */
    ~BiosManager() override = default;

    /*!
     * @brief Load all bios from bios.xml
     */
    void LoadFromFile();

    //! Get system count
    int SystemCount() const { return (int)mSystemBiosList.size(); }

    /*!
     * @brief Get bios list from the given system index
     * @param index System index
     * @return BiosList object
     */
    const BiosList& SystemBios(int index) const { return mSystemBiosList[index]; }

    /*!
     * @brief Lookup bios list for the given system
     * @param name Syste name
     * @return Bios list or nullptr if not found
     */
    const BiosList& SystemBios(const std::string& name);

    /*!
     * @brief Start scanning all bios and report result using the given interface
     * @param reporting Reporting interface
     */
    void Scan(IBiosScanReporting* reporting, bool sync = false);

    /*!
     * @brief Generate missing bios report in bios root folder
     */
    void GenerateReport() const;
};

//
// Created by bkg2k on 28/12/2019.
//

#ifndef EMULATIONSTATION_ALL_BIOSMANAGER_H
#define EMULATIONSTATION_ALL_BIOSMANAGER_H


#include <vector>
#include <utils/os/system/Thread.h>
#include <utils/os/system/Mutex.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include "BiosList.h"
#include "IBiosScanReporting.h"

class BiosManager : private Thread, public ISynchronousEvent
{
  private:
    //! Path to bios.xml file
    static constexpr const char* sBiosFilePath = "system/.emulationstation/es_bios.xml";

    //! Bios list per system
    std::vector<BiosList> mSystemBiosList;
    //! Sync'ed event sender
    SyncronousEvent mSender;
    //! Current scan's reporting interface (also flag for an already running scan)
    IBiosScanReporting* mReporting;

    /*
     * Thread implementation
     */

    /*!
     * Threaded scan implementation
     */
    void Run() override;

    /*
     * ISynchronousInterface
     */

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

  public:
    /*!
     * @brief Default bios
     */
    BiosManager();

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
     * @brief Start scanning all bios and report result using the given interface
     * @param reporting Reporting interface
     */
    void Scan(IBiosScanReporting* reporting);
};


#endif //EMULATIONSTATION_ALL_BIOSMANAGER_H

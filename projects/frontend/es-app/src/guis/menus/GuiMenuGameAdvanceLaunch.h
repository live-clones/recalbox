//
// Created by gugue_u on 02/04/2022.
//

#pragma once


#include "GuiMenuBase.h"
#include "systems/SystemManager.h"
#include "views/gamelist/IGameListView.h"
#include "emulators/run/GameLinkedData.h"

class GuiMenuGameAdvanceLaunch : public GuiMenuBase
  , private IOptionListComponent<Path>
{
public:

    GuiMenuGameAdvanceLaunch(WindowManager&window, SystemData& system);

  private:
    enum class Components
    {
        Patch,
        Region,
        Resolution,
        SaveState,
    };

    //! System reference
    SystemData& mSystem;
    //! Gamelist UI reference
    IGameListView& mGamelist;
    FileData& mFileData;



    //! Patch
    std::shared_ptr<OptionListComponent<Path>> mPatch;

    //! Get Patch List
    std::vector<ListEntry<Path>> GetPatchEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const Path& value) override;

    void AdvancedLaunch();
};



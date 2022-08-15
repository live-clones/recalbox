
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <views/gamelist/IGameListView.h>
#include "emulators/run/GameLinkedData.h"

class SystemManager;
class SystemData;
template<class T> class OptionListComponent;

class GuiMenuSoftpatchingLauncher : public GuiMenuBase
, private IOptionListComponent<Path>

{
  public:
    explicit GuiMenuSoftpatchingLauncher(WindowManager& window,
                                         FileData& game,
                                         std::list<Path>& patches,
                                         int lastChoice,
                                         const std::function<void()>& func1,
                                         const std::function<void(const Path&)>& func2);

  private:
    enum class Components
    {
      Patch,
    };

    //! Game reference
    FileData& mGame;
    std::list<Path>& mPatches;

    std::shared_ptr<OptionListComponent<Path>> mPaths;


    std::vector<ListEntry<Path>> GetPatchesEntries();

    void OptionListComponentChanged(int id, int index, const Path& value) override;
};




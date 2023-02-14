//
// Created by gugue_u on 04/01/2023.
//

#include <components/ComponentList.h>
#include "components/ComponentGrid.h"
#include <components/NinePatchComponent.h>
#include <components/OptionListComponent.h>
#include <themes/MenuThemeData.h>
#include "systems/SystemManager.h"
#include "games/SaveState.h"

class GuiSaveStates : public Gui
{
  public:
    GuiSaveStates(WindowManager& window, SystemManager& systemManager, FileData& game, const std::function<void(const std::string& slot)>& func, bool fromMenu);

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;

    void onSizeChanged() override;

    void Update(int deltaTime) override;

    void Render(const Transform4x4f& parentTrans) override;

    void PopulateGrid();

    void updateInformations(int i);

    void launch();

    void initGridsNStuff();

    void clear();

    SaveState GetStateAt(int index);

  private:

    enum class Sort
    {
      SLOT_ASC,
      SLOT_DESC
    };

    SystemManager& mSystemManager;

    NinePatchComponent mBackground;
    ComponentGrid mGrid;
    std::shared_ptr<MenuTheme> mMenuTheme;
    std::shared_ptr<ComponentGrid> mGridBody;
    std::shared_ptr<ComponentGrid> mGridLogoAndMD;
    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<TextComponent> mGameName;
    std::shared_ptr<ComponentList> mList;
    std::shared_ptr<ImageComponent> mThumbnail;
    std::shared_ptr<TextComponent> mStateField;
    std::shared_ptr<TextComponent> mModificationTimeField;
    std::list<SaveState> mSaveStates;
    FileData& mGame;
    bool mIsLibretro;
    bool mFromMenu;
    SaveState mCurrentState;
    Sort mSort;
    std::function<void(const std::string& slot)> mFunc;

    void Delete();

    void GetSaveStates();

    static bool asc(const SaveState& first, const SaveState& second)
    {
      if(first.GetIsAuto())
        return true;
      if(second.GetIsAuto())
        return false;

      return first.GetSlotNumber() < second.GetSlotNumber();
    }

    static bool desc(const SaveState& first, const SaveState& second)
    {
      if(first.GetIsAuto())
        return true;

      if(second.GetIsAuto())
        return false;

      return first.GetSlotNumber() > second.GetSlotNumber();
    }

    int GetSaveStateIndex(SaveState saveState);

    std::function<void()> func;

};
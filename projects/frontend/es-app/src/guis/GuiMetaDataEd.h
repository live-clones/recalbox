#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "GuiScraperSingleGameRun.h"

#include <systems/SystemData.h>
#include <views/gamelist/IGameListView.h>

class GuiMetaDataEd : public Gui, public GuiScraperSingleGameRun::IScrappingComplete
{
  public:
    class IMetaDataAction
    {
      public:
        virtual void Delete(IGameListView* gamelistview, FileData& game) = 0;
        virtual void Modified(IGameListView* gamelistview, FileData& game) = 0;
    };

    GuiMetaDataEd(WindowManager&window, SystemManager& systemManager, FileData& game,
                  IGameListView* gamelistview, IMetaDataAction* actions , bool main);

    bool ProcessInput(const InputCompactEvent& event) override;
    void onSizeChanged() override;
    bool getHelpPrompts(Help& help) override;

  private:
    void save();
    void fetch();
    void close(bool closeAllWindows);

    FileData& mGame;
    SystemManager& mSystemManager;

    NinePatchComponent mBackground;
    ComponentGrid mGrid;

    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<TextComponent> mSubtitle;
    std::shared_ptr<ComponentGrid> mHeaderGrid;
    std::shared_ptr<ComponentList> mList;
    std::shared_ptr<ComponentGrid> mButtons;

    std::vector< std::shared_ptr<Component> > mEditors;
    std::vector< const MetadataFieldDescriptor* > mMetaDataEditable;

    MetadataDescriptor& mMetaData;
    IGameListView* mGameListView;
    IMetaDataAction* mActions;

    /*
     * GuiScraperSingleGameRun::IScrappingCommplete
     */
    void ScrappingComplete(FileData& game) override;
};

#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "GuiGameScraper.h"

#include <functional>
#include <systems/SystemData.h>

class GuiMetaDataEd : public Gui, public GuiGameScraper::IScrappingComplete
{
  private:
    void save();
    void fetch();
    void close(bool closeAllWindows);

    FileData& mGame;

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
    std::function<void()> mSavedCallback;
    std::function<void()> mDeleteFunc;

    /*
     * GuiGameScraper::IScrappingCommplete
     */

    void ScrappingComplete(FileData& game) override;

  public:
    GuiMetaDataEd(Window&window, MetadataDescriptor& md, FileData& game,
                  const std::string& header, std::function<void()> savedCallback, std::function<void()> deleteFunc,
                  SystemData* system, bool main);

    bool ProcessInput(const InputCompactEvent& event) override;
    void onSizeChanged() override;
    bool getHelpPrompts(Help& help) override;
};

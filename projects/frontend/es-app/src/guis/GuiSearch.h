//
// Created by xizor on 05/10/2019.
//

#ifndef EMULATIONSTATION_ALL_GUISEARCH_H
#define EMULATIONSTATION_ALL_GUISEARCH_H

#include <components/ComponentList.h>
#include "components/ComponentGrid.h"
#include <components/NinePatchComponent.h>
#include <components/ButtonComponent.h>
#include <components/TextEditComponent.h>
#include <components/OptionListComponent.h>
#include <components/VideoComponent.h>
#include <themes/MenuThemeData.h>
#include "systems/SystemManager.h"

class GuiSearch : public Gui, public IGuiArcadeVirtualKeyboardInterface
{
  public:
    GuiSearch(WindowManager& window, SystemManager& systemManager);

    ~GuiSearch() override;

    float getButtonGridHeight() const;

    void updateSize();

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;

    void onSizeChanged() override;

    void Update(int deltaTime) override;

    void Render(const Transform4x4f& parentTrans) override;

    void PopulateGrid(const std::string& search);

    void populateGridMeta(int i);

    void launch();

    void GoToGame();

    void initGridsNStuff();

    void ResizeGridLogo();

    void clear();

  private:
    SystemManager& mSystemManager;

    NinePatchComponent mBackground;
    //full grid (entire frame)
    ComponentGrid mGrid;
    std::shared_ptr<MenuTheme> mMenuTheme;
    //grid for list & Meta
    std::shared_ptr<ComponentGrid> mGridMeta;
    //grid for logo + publisher and developer
    std::shared_ptr<ComponentGrid> mGridLogoAndMD;
    //3x3 grid to center logo
    std::shared_ptr<ComponentGrid> mGridLogo;
    std::shared_ptr<ComponentGrid> mButtonGrid;
    std::vector<std::shared_ptr<ButtonComponent> > mButtons;
    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<TextComponent> mText;
    std::shared_ptr<TextEditComponent> mSearch;
    std::shared_ptr<TextComponent> mMDDeveloperLabel;
    std::shared_ptr<TextComponent> mMDDeveloper;
    std::shared_ptr<TextComponent> mMDPublisherLabel;
    std::shared_ptr<TextComponent> mMDPublisher;
    std::shared_ptr<ComponentList> mList;
    std::shared_ptr<ImageComponent> mResultThumbnail;
    std::shared_ptr<ImageComponent> mResultSystemLogo;
    std::shared_ptr<VideoComponent> mResultVideo;
    std::shared_ptr<ScrollableContainer> mDescContainer;
    std::shared_ptr<TextComponent> mResultDesc;
    std::shared_ptr<OptionListComponent<FolderData::FastSearchContext>> mSearchChoices;
    FileData::List mSearchResults;
    SystemData* mSystemData;

    //! Just-open flag
    bool mJustOpen;

    /*!
     * @brief Called when the edited text change.
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text) final;

    /*!
     * @brief Called when the edited text is validated (Enter or Start)
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text) final;

    /*!
     * @brief Called when the edited text is cancelled.
     */
    void ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk) final;
};


#endif //EMULATIONSTATION_ALL_GUISEARCH_H

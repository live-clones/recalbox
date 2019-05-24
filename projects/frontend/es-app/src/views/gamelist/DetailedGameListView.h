#pragma once

#include "views/gamelist/BasicGameListView.h"
#include "components/ScrollableContainer.h"
#include "components/RatingComponent.h"
#include "components/DateTimeComponent.h"
#include "components/ImageVideoComponent.h"
#include "SystemData.h"

class DetailedGameListView : public BasicGameListView
{
public:
    DetailedGameListView(Window* window, FolderData* root, SystemData* system);

    virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme) override;

    virtual const char* getName() const override { return "detailed"; }

    virtual void updateInfoPanel() override;

protected:
    virtual void launch(FileData* game) override;

private:
    void initMDLabels();
    void initMDValues();

    ImageVideoComponent mImage;
    std::vector<ImageComponent *> mFolderContent;

    TextComponent mLblRating, mLblReleaseDate, mLblDeveloper, mLblPublisher, mLblGenre, mLblPlayers, mLblLastPlayed, mLblPlayCount, mLblFavorite;
    TextComponent mFolderName;

    RatingComponent mRating;
    DateTimeComponent mReleaseDate;
    TextComponent mDeveloper;
    TextComponent mPublisher;
    TextComponent mGenre;
    TextComponent mPlayers;
    DateTimeComponent mLastPlayed;
    TextComponent mPlayCount;
    TextComponent mFavorite;

    std::vector<TextComponent*> getMDLabels();
    std::vector<GuiComponent*> getMDValues();

    ScrollableContainer mDescContainer;
    TextComponent mDescription;

    SystemData* mSystem;

    bool switchDisplay(bool isGame);
    bool switchToFolderScrappedDisplay();
    std::vector<GuiComponent*> getFolderComponents();
    std::vector<GuiComponent*> getGameComponents(bool includeMainComponents = true);
    std::vector<GuiComponent*> getScrappedFolderComponents();
    void setFolderInfo(FolderData* folder);
    void setGameInfo(FileData* file);
    void setScrappedFolderInfo(FileData* file);
    //void getFolderGames(FileData* folder, FileData::List &output);
    void fadeOut(std::vector<GuiComponent*> comps, bool fadingOut);
};

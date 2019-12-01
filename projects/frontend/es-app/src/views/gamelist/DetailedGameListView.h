#pragma once

#include "views/gamelist/BasicGameListView.h"
#include "components/ScrollableContainer.h"
#include "components/RatingComponent.h"
#include "components/DateTimeComponent.h"
#include "components/ImageComponent.h"
#include "components/VideoComponent.h"
#include "systems/SystemData.h"

class DetailedGameListView : public BasicGameListView
{
public:
    DetailedGameListView(Window& window, SystemManager& systemManager, FolderData* root);

    void onThemeChanged(const ThemeData& theme) override;

    const char* getName() const override { return "detailed"; }

    void Update(int deltatime) override;

    void updateInfoPanel() override;

protected:
    void launch(FileData* game) override;

private:
    void initMDLabels();
    void initMDValues();

    ImageComponent mImage;
    VideoComponent mVideo;
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
    std::vector<Component*> getMDValues();

    ScrollableContainer mDescContainer;
    TextComponent mDescription;

    SystemData* mSystem;

    RecalboxConf& mSettings;

    bool switchDisplay(bool isGame);
    bool switchToFolderScrappedDisplay();
    std::vector<Component*> getFolderComponents();
    std::vector<Component*> getGameComponents(bool includeMainComponents = true);
    std::vector<Component*> getScrappedFolderComponents();
    void setFolderInfo(FolderData* folder);
    void setGameInfo(FileData* file);
    void setScrappedFolderInfo(FileData* file);
    //void getFolderGames(FileData* folder, FileData::List &output);
    static void fadeOut(const std::vector<Component*>& comps, bool fadingOut);
};

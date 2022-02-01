#pragma once

#include "views/gamelist/BasicGameListView.h"
#include "components/ScrollableContainer.h"
#include "components/RatingComponent.h"
#include "components/DateTimeComponent.h"
#include "components/ImageComponent.h"
#include "components/VideoComponent.h"
#include "systems/SystemData.h"

class DetailedGameListView : public BasicGameListView, public ITextListComponentOverlay<FileData*>
{
public:
    DetailedGameListView(WindowManager& window, SystemManager& systemManager, SystemData& system);

    ~DetailedGameListView();

    void onThemeChanged(const ThemeData& theme) override;

    const char* getName() const override { return "detailed"; }

    void Update(int deltatime) override;

    void DoUpdateGameInformation() final;

protected:
    void launch(FileData* game) override;
    void clean() override { mVideo.setVideo(Path::Empty, 0, 0); }

private:
    void initMDLabels();
    void initMDValues();

    ImageComponent mImage;
    ImageComponent mNoImage;
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
    ImageComponent mRegion1;
    ImageComponent mRegion2;
    ImageComponent mRegion3;
    ImageComponent mRegion4;

    std::vector<TextComponent*> getMDLabels();
    std::vector<Component*> getMDValues();

    ScrollableContainer mDescContainer;
    TextComponent mDescription;

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

    /*
     * ITextListComponentOverlay<FileData*> implementation
     */

    /*!
     * @brief Apply (draw) an overlay in the given item rectangle and adjust rectangle position/size
     * so that the text won't draw over the overlay if required
     * @param position Top/Left of the item rectangle
     * @param size  Width/Height of the item rectangle
     * @param data Linked data
     */
    void OverlayApply(const Vector2f& position, const Vector2f& size, FileData*& data, unsigned int& color) override;

    /*!
     * @brief Get the left offset (margin to the text) if any
     * @return left offset
     */
    float OverlayGetLeftOffset() override { return 0.0f; }

    /*!
     * @brief Get the right offset (margin from text to right limit) if any
     * @return right offset
     */
    float OverlayGetRightOffset() override;
};

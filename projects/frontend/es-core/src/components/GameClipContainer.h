//
// Created by gugue_U on 17/10/2020
//

#pragma once

#include <vector>
#include <random>
#include <games/FileData.h>
#include <systems/PlatformId.h>
#include <systems/SystemManager.h>
#include <utils/datetime/HighResolutionTimer.h>
#include <components/TextComponent.h>
#include <components/VideoComponent.h>
#include <components/RatingComponent.h>
#include <components/ScrollableContainer.h>
#include <components/ClippingContainer.h>

#include <themes/ThemeExtras.h>
#include <components/DateTimeComponent.h>
#include "RecalboxConf.h"

class GameClipContainer : public Gui
{

  private:

    WindowManager& mWindow;

    void initMDLabels();

    void initMDValues();

    std::vector<TextComponent*> getMDLabels();

    std::vector<Component*> getMDValues();

    VideoComponent mVideo;
    ImageComponent mImage;
    ImageComponent mThumbnail;
    ImageComponent mRecalboxLogo;
    ClippingContainer mClippingContainer;
    ImageComponent mClippingImage;

    TextComponent mLbGameName, mLbSystemName, mLblRating, mLblReleaseDate, mLblDeveloper, mLblPublisher, mLblGenre, mLblPlayers, mLblLastPlayed, mLblPlayCount, mLblFavorite;
    TextComponent mGameName;
    TextComponent mSystemName;
    RatingComponent mRating;
    DateTimeComponent mReleaseDate;
    TextComponent mDeveloper;
    TextComponent mPublisher;
    TextComponent mGenre;
    TextComponent mPlayers;
    DateTimeComponent mLastPlayed;
    TextComponent mPlayCount;
    TextComponent mFavorite;
    ImageComponent mFavoriteIcon;

    ScrollableContainer mDescContainer;
    TextComponent mDescription;
    TextComponent mHeaderText;
    ImageComponent mHeaderImage;
    ImageComponent mBackground;

    ThemeExtras mThemeExtras;
    FileData* mGame;
    SystemData* mSystem;

    void onThemeChanged(const ThemeData& theme);

    void initComponents();

  public:

    explicit GameClipContainer(WindowManager& window);

    void Render(const Transform4x4f& parentTrans) override;

    void setGameInfo(FileData* game);

    void CleanVideo();

    Vector2f getVideoCenter();

    bool getHelpPrompts(Help& help);
};
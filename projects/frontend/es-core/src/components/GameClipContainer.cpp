//
// Created by gugue_U on 17/10/2020
//

#include <recalbox/RecalboxSystem.h>
#include "components/GameClipContainer.h"
#include "utils/Log.h"
#include <views/ViewController.h>
#include <audio/AudioManager.h>
#include "utils/locale/LocaleHelper.h"

GameClipContainer::GameClipContainer(WindowManager& window)
  : Gui(window), mWindow(window), mVideo(window), mImage(window), mThumbnail(window), mRecalboxLogo(window),
    mClippingContainer(window, 300, 1000), mClippingImage(window), mLbGameName(window), mLbSystemName(window), mLblRating(window),
    mLblReleaseDate(window), mLblDeveloper(window), mLblPublisher(window), mLblGenre(window), mLblPlayers(window),
    mLblLastPlayed(window), mLblPlayCount(window), mLblFavorite(window), mGameName(window), mSystemName(window),
    mRating(window, 0.f), mReleaseDate(window), mDeveloper(window), mPublisher(window), mGenre(window), mPlayers(window),
    mLastPlayed(window), mPlayCount(window), mFavorite(window), mFavoriteIcon(window), mDescContainer(window), mDescription(window),
    mHeaderText(window), mHeaderImage(window), mBackground(window), mThemeExtras(window), mGame(nullptr),
    mSystem(nullptr)
{

  setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  addChild(&mVideo);
  addChild(&mImage);
  addChild(&mThumbnail);
  addChild(&mRecalboxLogo);
  if (RecalboxConf::Instance().GetShowGameClipClippingItem())
  {
    addChild(&mClippingContainer);
    mClippingContainer.addChild(&mClippingImage);
  }
  addChild(&mBackground);

  addChild(&mDescContainer);
  mDescContainer.addChild(&mDescription);

  // values
  addChild(&mGameName);
  addChild(&mSystemName);
  addChild(&mRating);
  addChild(&mReleaseDate);
  addChild(&mDeveloper);
  addChild(&mPublisher);
  addChild(&mGenre);
  addChild(&mPlayers);
  mLastPlayed.setDisplayMode(DateTimeComponent::Display::RelativeToNow);
  addChild(&mLastPlayed);
  addChild(&mPlayCount);

  // labels
  mLbGameName.setText(_("Game") + ": ");
  addChild(&mLbGameName);
  mLbSystemName.setText(_("System") + ": ");
  addChild(&mLbSystemName);
  mLblRating.setText(_("Rating") + ": ");
  addChild(&mLblRating);
  mLblReleaseDate.setText(_("Released") + ": ");
  addChild(&mLblReleaseDate);
  mLblDeveloper.setText(_("Developer") + ": ");
  addChild(&mLblDeveloper);
  mLblPublisher.setText(_("Publisher") + ": ");
  addChild(&mLblPublisher);
  mLblGenre.setText(_("Genre") + ": ");
  addChild(&mLblGenre);
  mLblPlayers.setText(_("Players") + ": ");
  addChild(&mLblPlayers);
  mLblLastPlayed.setText(_("Last played") + ": ");
  addChild(&mLblLastPlayed);
  mLblPlayCount.setText(_("Times played") + ": ");
  addChild(&mLblPlayCount);
}

void GameClipContainer::initComponents()
{
  // init all labels and values with theme default values
  // and disabled them for default view "fullsceen"
  initMDValues();
  initMDLabels();

  mDescription.setSize(mDescContainer.getSize().x(), 0);
  mDescContainer.setThemeDisabled(true);
  mDescription.setPosition(0, 0);

  mHeaderText.setDefaultZIndex(50);
  mHeaderText.setThemeDisabled(true);
  mHeaderImage.setDefaultZIndex(50);
  mHeaderImage.setThemeDisabled(true);

  mThumbnail.setDefaultZIndex(50);
  mThumbnail.setThemeDisabled(true);

  // init visible components for default view
  const float padding = 0.01f;
  const float verticalPadding = mSize.y() * padding;
  const float horizontalPadding = mSize.x() * padding;

  mVideo.setOrigin(0.5, 0.5);
  mVideo.setPosition(mSize.x() * 0.5f, mSize.y() * 0.5f);
  mVideo.setMaxSize(mSize.x(), mSize.y());
  mVideo.setDefaultZIndex(30);

  if (RecalboxConf::Instance().GetShowGameClipClippingItem())
  {
    mClippingContainer.setSize(mSize);
    mClippingContainer.setDefaultZIndex(50);
    mClippingContainer.setZIndex(50);

    mClippingImage.setImage(Path(":/press_start.svg"));
    mClippingImage.setSize(mSize.x() * 0.12f, mSize.y() * 0.12f);
    mClippingImage.setOrigin(0.5, 0.5);
  }

  mRecalboxLogo.setImage(Path(":/Recalbox_logo_White.svg"));
  mRecalboxLogo.setOrigin(1, 0);
  mRecalboxLogo.setPosition(mSize.x() - horizontalPadding, 0 + horizontalPadding);
  mRecalboxLogo.setSize(mSize.x() * 0.09f, mSize.x() * 0.09f);
  mRecalboxLogo.setZIndex(50);
  mRecalboxLogo.setDefaultZIndex(50);

  mBackground.setImage(Path(":/white.png"));
  mBackground.setOrigin(0, 1);
  mBackground.setPosition(0, mSize.y());
  mBackground.setSize(mSize.x(), mSize.y() * 0.13f);
  mBackground.setColor(0x9F9F9FCC);
  mBackground.setZIndex(40);
  mBackground.setDefaultZIndex(0);

  mImage.setOrigin(0, 1);
  mImage.setPosition(mSize.x() * 0.025f, mSize.y() * 0.92f);
  mImage.setMaxSize(mSize.x() * 0.2f, mSize.y() * 0.4f);
  mImage.setZIndex(50);
  mImage.setDefaultZIndex(40);

  mGameName.setFont(Font::get(FONT_SIZE_MEDIUM));
  mGameName.setPosition(mSize.x() * 0.97f, mSize.y() * 0.91f);
  mGameName.setOrigin(1, 1);
  mGameName.setSize(mSize.x() * 0.6f, mSize.y() * 0.03f);
  mGameName.setZIndex(50);
  mGameName.setDefaultZIndex(50);
  mGameName.setHorizontalAlignment(TextAlignment::Right);
  mGameName.setColor(0xFFFFFFFF);

  mFavoriteIcon.setImage(Path(":/heart_filled.svg"));
  mFavoriteIcon.setOrigin(0, 1);
  mFavoriteIcon.setPosition(mSize.x() * 0.975f, mSize.y() * 0.91f);
  mFavoriteIcon.setMaxSize(mSize.x() * 0.018f, mSize.x() * 0.018f);
  mFavoriteIcon.setZIndex(50);
  mFavoriteIcon.setDefaultZIndex(50);

  float systemPosY = mGameName.getPosition().y() + mGameName.getSize().y() + verticalPadding;
  mSystemName.setPosition(mSize.x() * 0.97f, systemPosY);
  mSystemName.setOrigin(1, 1);
  mSystemName.setSize(mSize.x() * 0.7f, mSize.y() * 0.03f);
  mSystemName.setZIndex(50);
  mSystemName.setDefaultZIndex(50);
  mSystemName.setHorizontalAlignment(TextAlignment::Right);
  mSystemName.setFont(Font::get(FONT_SIZE_SMALL));
  mSystemName.setColor(0xFFFFFFFF);

  float releasePosY = mSystemName.getPosition().y() + mSystemName.getSize().y() + verticalPadding;
  mReleaseDate.setPosition(mSize.x() * 0.97f, releasePosY);
  mReleaseDate.setSize(mSize.x() * 0.7f, mSize.y() * 0.03f);
  mReleaseDate.setZIndex(50);
  mReleaseDate.setDefaultZIndex(50);
  mReleaseDate.setOrigin(1, 1);
  mReleaseDate.setHorizontalAlignment(TextAlignment::Right);
  mReleaseDate.setFont(Font::get(FONT_SIZE_SMALL));
  mReleaseDate.setColor(0xFFFFFFFF);
  mReleaseDate.setDisplayMode(DateTimeComponent::Display::Year);
}

void GameClipContainer::initMDLabels()
{
  std::vector<TextComponent*> labels = getMDLabels();

  for (auto& textLabel : labels)
  {
    textLabel->setFont(Font::get(FONT_SIZE_SMALL));
    textLabel->setThemeDisabled(true);
    textLabel->setDefaultZIndex(50);
  }
}

void GameClipContainer::initMDValues()
{
  std::vector<TextComponent*> labels = getMDLabels();
  std::vector<Component*> values = getMDValues();

  std::shared_ptr<Font> defaultFont = Font::get(FONT_SIZE_SMALL);
  mRating.setSize(defaultFont->getHeight() * 5.0f, defaultFont->getHeight());
  mDeveloper.setFont(defaultFont);
  mPublisher.setFont(defaultFont);
  mGenre.setFont(defaultFont);
  mPlayers.setFont(defaultFont);
  mLastPlayed.setFont(defaultFont);
  mPlayCount.setFont(defaultFont);
  mFavorite.setFont(defaultFont);
  mDescription.setFont(defaultFont);

  for (unsigned int i = 0; i < (unsigned int) values.size(); i++)
  {
    values[i]->setDefaultZIndex(50);
    values[i]->setThemeDisabled(true);
  }
}

void GameClipContainer::onThemeChanged(const ThemeData& theme)
{
  initComponents();
  if (theme.getGameClipView() != ThemeData::getNoTheme())
  {

    mBackground.applyTheme(theme, GameClipView::getName(), "background", ThemeProperties::All);
    mHeaderImage.applyTheme(theme, GameClipView::getName(), "logo", ThemeProperties::All);
    mHeaderText.applyTheme(theme, GameClipView::getName(), "logoText", ThemeProperties::All);

    // Remove old theme extras
    for (auto* extra : mThemeExtras.getmExtras())
    {
      removeChild(extra);
    }
    mThemeExtras.getmExtras().clear();
    mThemeExtras.setExtras(ThemeData::makeExtras(theme, GameClipView::getName(), mWindow));
    mThemeExtras.sortExtrasByZIndex();

    // Add new theme extras
    for (auto* extra : mThemeExtras.getmExtras())
    {
      addChild(extra);
    }

    if (mHeaderImage.hasImage())
    {
      removeChild(&mHeaderText);
      addChild(&mHeaderImage);
    }
    else
    {
      addChild(&mHeaderText);
      removeChild(&mHeaderImage);
    }

    mImage.applyTheme(theme, GameClipView::getName(), "md_image",
                      ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex |
                      ThemeProperties::Rotation);

    mThumbnail.applyTheme(theme, GameClipView::getName(), "md_thumbnail",
                          ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex |
                          ThemeProperties::Rotation);

    if (mSystem->HasFavoritesInTheme())
    {
      mFavoriteIcon.applyTheme(theme, GameClipView::getName(), "favoriteIcon",
                               ThemeProperties::All);
    }

    mRecalboxLogo.applyTheme(theme, GameClipView::getName(), "recalboxLogo", ThemeProperties::All);

    mClippingImage.applyTheme(mGame->System().Theme(), GameClipView::getName(), "clippingImage",
                              ThemeProperties::Path | ThemeProperties::Size);

    std::vector<TextComponent*> labels = getMDLabels();
    std::vector<std::string> names({
                                     "md_lbl_gameName",
                                     "md_lbl_systemName",
                                     "md_lbl_rating",
                                     "md_lbl_releasedate",
                                     "md_lbl_developer",
                                     "md_lbl_publisher",
                                     "md_lbl_genre",
                                     "md_lbl_players",
                                     "md_lbl_lastplayed",
                                     "md_lbl_playcount"
                                   });

    if (mSystem->HasFavoritesInTheme())
    {
      names.emplace_back("md_lbl_favorite");
    }

    assert(names.size() == labels.size());
    for (unsigned int i = 0; i < (unsigned int) labels.size(); i++)
    {
      labels[i]->applyTheme(theme, GameClipView::getName(), names[i], ThemeProperties::All);
    }

    std::vector<Component*> values = getMDValues();
    names = {
      "md_gameName",
      "md_rating",
      "md_releasedate",
      "md_developer",
      "md_publisher",
      "md_genre",
      "md_players",
      "md_lastplayed",
      "md_playcount",
    };

    if (mSystem->HasFavoritesInTheme())
    {
      names.emplace_back("md_favorite");
    }

    assert(names.size() == values.size());
    for (unsigned int i = 0; i < (unsigned int) values.size(); i++)
    {
      values[i]->applyTheme(theme, GameClipView::getName(), names[i], ThemeProperties::All ^ ThemeProperties::Text);
    }

    mSystemName.applyTheme(theme, GameClipView::getName(), "md_systemName",
    ThemeProperties::All);
    mDescContainer.applyTheme(theme, GameClipView::getName(), "md_description",
                              ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex);
    mDescription.setSize(mDescContainer.getSize().x(), 0.0);
    mDescription.applyTheme(theme, GameClipView::getName(), "md_description", ThemeProperties::All ^
                                                                              (ThemeProperties::Position |
                                                                               ThemeProperties::Size |
                                                                               ThemeProperties::Origin |
                                                                               ThemeProperties::Text));

    mVideo.applyTheme(theme, GameClipView::getName(), "md_video",
                      ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex |
                      ThemeProperties::Rotation);

  }
  sortChildren();
}

void GameClipContainer::Render(const Transform4x4f& parentTrans)
{
  if (!mVideo.isDiplayed())
  {
    mVideo.Render(parentTrans);
    return;
  }
  mFavorite.setValue(mGame->Metadata().FavoriteAsString());
  if(mGame->Metadata().Favorite())
  {
    addChild(&mFavoriteIcon);
  }
  else
  {
    removeChild(&mFavoriteIcon);
  }

  Vector2f videoCenter = mVideo.getCenter();
  ThemeData theme = mGame->System().Theme();
  mClippingImage.setPosition(videoCenter.x(), videoCenter.y());

  renderChildren(parentTrans);
}

// element order need to follow the one in onThemeChanged
std::vector<TextComponent*> GameClipContainer::getMDLabels()
{
  std::vector<TextComponent*> ret;
  ret.push_back(&mLbGameName);
  ret.push_back(&mLbSystemName);
  ret.push_back(&mLblRating);
  ret.push_back(&mLblReleaseDate);
  ret.push_back(&mLblDeveloper);
  ret.push_back(&mLblPublisher);
  ret.push_back(&mLblGenre);
  ret.push_back(&mLblPlayers);
  ret.push_back(&mLblLastPlayed);
  ret.push_back(&mLblPlayCount);
  if (mSystem->HasFavoritesInTheme())
  {
    ret.push_back(&mLblFavorite);
  }
  return ret;
}

// element order need to follow the one in onThemeChanged
std::vector<Component*> GameClipContainer::getMDValues()
{
  std::vector<Component*> ret;
  ret.push_back(&mGameName);
  ret.push_back(&mRating);
  ret.push_back(&mReleaseDate);
  ret.push_back(&mDeveloper);
  ret.push_back(&mPublisher);
  ret.push_back(&mGenre);
  ret.push_back(&mPlayers);
  ret.push_back(&mLastPlayed);
  ret.push_back(&mPlayCount);
  if (mSystem->HasFavoritesInTheme())
  {
    ret.push_back(&mFavorite);
  }
  return ret;
}

void GameClipContainer::setGameInfo(FileData* game)
{
  mGame = game;
  mSystem = &mGame->System();
  mSystemName.setValue(mGame->System().FullName());

  onThemeChanged(mSystem->Theme());

  mGameName.setValue(mGame->Metadata().Name());
  mRating.setValue(mGame->Metadata().RatingAsString());
  mReleaseDate.setValue(mGame->Metadata().ReleaseDateAsString());
  mDeveloper.setValue(mGame->Metadata().Developer());
  mPublisher.setValue(mGame->Metadata().Publisher());
  mGenre.setValue(mGame->Metadata().Genre());
  mPlayers.setValue(mGame->Metadata().PlayersAsString());
  mLastPlayed.setValue(mGame->Metadata().LastPlayedAsString());
  mPlayCount.setValue(mGame->Metadata().PlayCountAsString());
  mVideo.setVideo(mGame->Metadata().Video(), 0, 1, AudioModeTools::CanDecodeVideoSound());

  mImage.setImage(mGame->Metadata().Image());
  mThumbnail.setImage(mGame->Metadata().Thumbnail());
  mDescription.setText(mGame->Metadata().Description());
  mDescContainer.reset();

  if (mSystem->HasFavoritesInTheme())
  {
    mLblFavorite.setText(_("Favorite") + ": ");
    addChild(&mLblFavorite);
    addChild(&mFavorite);
  }
}

void GameClipContainer::CleanVideo()
{
  mVideo.setVideo(Path::Empty, 0, 0);
}

Vector2f GameClipContainer::getVideoCenter()
{
  return mVideo.getCenter();
}

bool GameClipContainer::getHelpPrompts(Help& help)
{
  if(RecalboxConf::Instance().GetShowGameClipHelpItems())
    help.Set(Help::Cancel(), _("QUIT"))
      .Set(HelpType::Start, _("LAUNCH"))
      .Set(HelpType::Y, mGame->Metadata().Favorite() ? _("Remove from favorite") : _( "Favorite"))
      .Set(HelpType::LeftRight, _("CHANGE"))
      .Set(HelpType::Select, _("GO TO GAME"));

  return true;
}
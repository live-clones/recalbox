#include <RecalboxConf.h>
#include <VideoEngine.h>
#include "views/gamelist/DetailedGameListView.h"
#include "views/ViewController.h"
#include "animations/LambdaAnimation.h"
#include "utils/locale/LocaleHelper.h"
#include "scraping/ScraperSeamless.h"

DetailedGameListView::DetailedGameListView(WindowManager&window, SystemManager& systemManager, SystemData& system)
  : BasicGameListView(window, systemManager, system)
  , mImage(window)
  , mNoImage(window)
  , mVideo(window)
  , mLblRating(window)
  , mLblReleaseDate(window)
  , mLblDeveloper(window)
  , mLblPublisher(window)
  , mLblGenre(window)
  , mLblPlayers(window)
  , mLblLastPlayed(window)
  , mLblPlayCount(window)
  , mLblFavorite(window)
  , mFolderName(window)
  , mRating(window, 0.f)
  , mReleaseDate(window)
  , mDeveloper(window)
  , mPublisher(window)
  , mGenre(window)
  , mPlayers(window)
  , mLastPlayed(window)
  , mPlayCount(window)
  , mFavorite(window)
  , mDescContainer(window)
  , mDescription(window)
  , mBusy(window)
  , mSettings(RecalboxConf::Instance())
  , mFadeBetweenImage(-1)
{
  //mList.SetOverlayInterface(this);

  const float padding = 0.01f;

  mList.setPosition(mSize.x() * (0.50f + padding), mList.getPosition().y());
  mList.setSize(mSize.x() * (0.50f - padding), mList.getSize().y());
  mList.setAlignment(HorizontalAlignment::Left);
  /*mList.setCursorChangedCallback([&](const CursorState& state)
                                 {
                                   (void) state;
                                   updateInfoPanel();
                                 });*/

  // folder components
  for (int i = 3 * 3; --i >= 0; )
  {
    auto* img = new ImageComponent(window);
    addChild(img); // normalised functions required to be added first
    img->setOrigin(0.5f, 0.5f);
    img->setNormalisedMaxSize(0.4f, 0.4f);
    img->setDefaultZIndex(30);
    img->setZIndex(30);
    mFolderContent.push_back(img);
  }

  addChild(&mFolderName);
  mFolderName.setDefaultZIndex(40);

  // image
  mImage.setOrigin(0.5f, 0.5f);
  mImage.setPosition(mSize.x() * 0.25f, mList.getPosition().y() + mSize.y() * 0.2125f);
  mImage.setMaxSize(mSize.x() * (0.50f - 2 * padding), mSize.y() * 0.4f);
  mImage.setDefaultZIndex(30);

  // no image
  mNoImage.setOrigin(mImage.getOrigin());
  mNoImage.setPosition(mImage.getPosition());
  mNoImage.setMaxSize(mImage.getSize());
  mNoImage.setDefaultZIndex(30);

  addChild(&mNoImage);
  addChild(&mImage);

  // video
  mVideo.setOrigin(0.5f, 0.5f);
  mVideo.setPosition(mSize.x() * 0.25f, mList.getPosition().y() + mSize.y() * 0.2125f);
  mVideo.setMaxSize(mSize.x() * (0.50f - 2 * padding), mSize.y() * 0.4f);
  mVideo.setDefaultZIndex(30);
  addChild(&mVideo);

  // Busy
  mBusy.setPosition(mImage.getPosition());
  mBusy.setSize(mImage.getSize());
  mBusy.setText(_("UPDATING..."));

  // metadata labels + values
  mLblRating.setText(_("Rating") + ": ");
  addChild(&mLblRating);
  addChild(&mRating);
  mLblReleaseDate.setText(_("Released") + ": ");
  addChild(&mLblReleaseDate);
  addChild(&mReleaseDate);
  mLblDeveloper.setText(_("Developer") + ": ");
  addChild(&mLblDeveloper);
  addChild(&mDeveloper);
  mLblPublisher.setText(_("Publisher") + ": ");
  addChild(&mLblPublisher);
  addChild(&mPublisher);
  mLblGenre.setText(_("Genre") + ": ");
  addChild(&mLblGenre);
  addChild(&mGenre);
  mLblPlayers.setText(_("Players") + ": ");
  addChild(&mLblPlayers);
  addChild(&mPlayers);
  mLblLastPlayed.setText(_("Last played") + ": ");
  addChild(&mLblLastPlayed);
  mLastPlayed.setDisplayMode(DateTimeComponent::Display::RelativeToNow);
  addChild(&mLastPlayed);
  mLblPlayCount.setText(_("Times played") + ": ");
  addChild(&mLblPlayCount);
  addChild(&mPlayCount);
  if (mSystem.HasFavoritesInTheme())
  {
    mLblFavorite.setText(_("Favorite") + ": ");
    addChild(&mLblFavorite);
    addChild(&mFavorite);
  }

  for (int i = 4; --i >= 0; )
  {
    auto* img = new ImageComponent(window);
    addChild(img); // normalised functions required to be added first
    img->setDefaultZIndex(40);
    img->setThemeDisabled(true);
    mRegions.push_back(img);
  }

  mDescContainer.setPosition(mSize.x() * padding, mSize.y() * 0.65f);
  mDescContainer.setSize(mSize.x() * (0.50f - 2 * padding), mSize.y() - mDescContainer.getPosition().y());
  mDescContainer.setAutoScroll(true);
  mDescContainer.setDefaultZIndex(40);
  addChild(&mDescContainer);

  mDescription.setFont(Font::get(FONT_SIZE_SMALL));
  mDescription.setSize(mDescContainer.getSize().x(), 0);
  mDescContainer.addChild(&mDescription);

  initMDLabels();
  initMDValues();
  updateInfoPanel();
}

void DetailedGameListView::onThemeChanged(const ThemeData& theme)
{
  BasicGameListView::onThemeChanged(theme);

  for (int i = 0; i < (int) mRegions.size(); i++)
  {
    char strbuf[256];
    snprintf(strbuf, 256, "md_region%d", i + 1);
    mRegions[i]->applyTheme(theme, getName(), strbuf,
                                  ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex | ThemeProperties::Path);
  }

  mImage.applyTheme(theme, getName(), "md_image", ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex | ThemeProperties::Rotation);
  mNoImage.applyTheme(theme, getName(), "md_image", ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex | ThemeProperties::Rotation);
  mNoImage.applyTheme(theme, getName(), "default_image_path", ThemeProperties::Path);
  mVideo.applyTheme(theme, getName(), "md_video", ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex | ThemeProperties::Rotation);

  initMDLabels();
  std::vector<TextComponent*> labels = getMDLabels();
  std::vector<std::string> names({
                                   "md_lbl_rating",
                                   "md_lbl_releasedate",
                                   "md_lbl_developer",
                                   "md_lbl_publisher",
                                   "md_lbl_genre",
                                   "md_lbl_players",
                                   "md_lbl_lastplayed",
                                   "md_lbl_playcount"
                                 });

  if (mSystem.HasFavoritesInTheme())
  {
    names.push_back("md_lbl_favorite");
  }

  assert(names.size() == labels.size());
  for (unsigned int i = 0; i < (unsigned int)labels.size(); i++)
  {
    labels[i]->applyTheme(theme, getName(), names[i], ThemeProperties::All);
  }

  initMDValues();
  std::vector<Component*> values = getMDValues();
  names = {
    "md_rating",
    "md_releasedate",
    "md_developer",
    "md_publisher",
    "md_genre",
    "md_players",
    "md_lastplayed",
    "md_playcount"
  };

  if (mSystem.HasFavoritesInTheme())
  {
    names.push_back("md_favorite");
  }

  names.push_back("md_folder_name");
  values.push_back(&mFolderName);

  assert(names.size() == values.size());
  for (unsigned int i = 0; i < (unsigned int)values.size(); i++)
  {
    values[i]->applyTheme(theme, getName(), names[i], ThemeProperties::All ^ ThemeProperties::Text);
  }

  mDescContainer.applyTheme(theme, getName(), "md_description", ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex);
  mDescription.setSize(mDescContainer.getSize().x(), 0);
  mDescription.applyTheme(theme, getName(), "md_description",
                          ThemeProperties::All ^ (ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::Origin | ThemeProperties::Text));
  mBusy.SetFont(mDescription.getFont());

  if (theme.isFolderHandled())
  {
    char strbuf[256];
    mFolderName.applyTheme(theme, getName(), "md_folder_name", ThemeProperties::All);
    for (int i = 0; i < (int) mFolderContent.size(); i++)
    {
      snprintf(strbuf, 256, "md_folder_image_%d", i);
      mFolderContent[i]->applyTheme(theme, getName(), strbuf,
                                    ThemeProperties::Position | ThemeProperties::Size | ThemeProperties::ZIndex | ThemeProperties::Rotation);
    }
  }
  else
  {
    // backward compatibility
    auto size = mImage.getSize().isZero() ? mImage.getTargetSize() : mImage.getSize();
    float minSize = Math::min(size.x(), size.y());
    float left = mImage.getPosition().x() - mImage.getOrigin().x() * minSize;
    float top = mImage.getPosition().y() - mImage.getOrigin().y() * minSize;

    mFolderName.setPosition(left, top);
    mFolderName.setZIndex(40);
    mFolderName.setFont(Font::get(FONT_SIZE_EXTRASMALL));
    mFolderName.setColor(0xFFFFFFFF);

    const unsigned int grid = 3; // 3 x 3
    const float relativeMarge = 0.1f;

    float labelHeight = 2.5f * Font::get(FONT_SIZE_EXTRASMALL)->getLetterHeight();
    top += labelHeight;

    minSize = Math::min(size.x(), size.y() - labelHeight);

    const float imgSize = minSize / (grid + 2.0f * relativeMarge);

    // centering with unused space
    left += 0.5f * (minSize - grid * imgSize - 2.0f * relativeMarge);

    for (unsigned int x = 0; x < grid; x++)
    {
      for (unsigned int y = 0; y < grid; y++)
      {
        ImageComponent* img = mFolderContent[x + y * grid];
        img->setMaxSize(imgSize, imgSize);
        img->setPosition(left + imgSize * img->getOrigin().x() + (float)x * (1 + relativeMarge) * imgSize,
                         top + imgSize * img->getOrigin().y() + (float)y * (1 + relativeMarge) * imgSize);
        img->setZIndex(30);
      }
    }
  }

  sortChildren();
}

void DetailedGameListView::initMDLabels()
{
  std::vector<TextComponent*> components = getMDLabels();

  const unsigned int colCount = 2;
  const unsigned int rowCount = (unsigned int) (components.size() / 2);

  Vector3f start(mSize.x() * 0.01f, mSize.y() * 0.625f, 0.0f);

  const float colSize = (mSize.x() * 0.48f) / colCount;
  const float rowPadding = 0.01f * mSize.y();

  for (unsigned int i = 0; i < (unsigned int)components.size(); i++)
  {
    const unsigned int row = i % rowCount;
    Vector3f pos(0.0f, 0.0f, 0.0f);
    if (row == 0)
    {
      pos = start + Vector3f(colSize * ((float)i / (float)rowCount), 0, 0);
    }
    else
    {
      // work from the last component
      Component* lc = components[i - 1];
      pos = lc->getPosition() + Vector3f(0, lc->getSize().y() + rowPadding, 0);
    }

    components[i]->setFont(Font::get(FONT_SIZE_SMALL));
    components[i]->setPosition(pos);
    components[i]->setDefaultZIndex(40);
  }
}

void DetailedGameListView::initMDValues()
{
  std::vector<TextComponent*> labels = getMDLabels();
  std::vector<Component*> values = getMDValues();

  std::shared_ptr<Font> defaultFont = Font::get(FONT_SIZE_SMALL);
  mRating.setSize(defaultFont->getHeight() * 5.0f, defaultFont->getHeight());
  mReleaseDate.setFont(defaultFont);
  mDeveloper.setFont(defaultFont);
  mPublisher.setFont(defaultFont);
  mGenre.setFont(defaultFont);
  mPlayers.setFont(defaultFont);
  mLastPlayed.setFont(defaultFont);
  mPlayCount.setFont(defaultFont);
  mFavorite.setFont(defaultFont);

  float bottom = 0.0f;

  const float colSize = (mSize.x() * 0.48f) / 2;
  for (unsigned int i = 0; i < (unsigned int)labels.size(); i++)
  {
    const float heightDiff = (labels[i]->getSize().y() - values[i]->getSize().y()) / 2;
    values[i]->setPosition(labels[i]->getPosition() + Vector3f(labels[i]->getSize().x(), heightDiff, 0));
    values[i]->setSize(colSize - labels[i]->getSize().x(), values[i]->getSize().y());
    values[i]->setDefaultZIndex(40);

    float testBot = values[i]->getPosition().y() + values[i]->getSize().y();
    if (testBot > bottom)
      bottom = testBot;
  }

  mDescContainer.setPosition(mDescContainer.getPosition().x(), bottom + mSize.y() * 0.01f);
  mDescContainer.setSize(mDescContainer.getSize().x(), mSize.y() - mDescContainer.getPosition().y());
}

void DetailedGameListView::DoUpdateGameInformation(bool update)
{
  FileData* file = (mList.size() == 0 || mList.isScrolling()) ? nullptr : mList.getSelected();

  if (file == nullptr)
  {
    VideoEngine::Instance().StopVideo(false);
    fadeOut(getFolderComponents(), true);
    fadeOut(getGameComponents(), true);
  }
  else
  {
    const bool isFolder = file->IsFolder();
    const bool hasImage = !file->Metadata().Image().IsEmpty();

    if (hasImage && isFolder)
    {
      setScrapedFolderInfo(file);
      switchToFolderScrapedDisplay();
    }
    else
    {
       if (isFolder)
       {
         setFolderInfo((FolderData*) file);
         for(int i = (int)mRegions.size(); --i >= 0; )
           mRegions[i]->setImage(Path());
       }
       else
        setGameInfo(file, update);
      switchDisplay(!isFolder);
    }
  }

  mWindow.UpdateHelpSystem();
}

bool DetailedGameListView::switchToFolderScrapedDisplay()
{
  fadeOut(getGameComponents(false), true);
  fadeOut(getFolderComponents(), true);
  fadeOut(getScrapedFolderComponents(), false);
  return true;
}

bool DetailedGameListView::switchDisplay(bool isGame)
{
  fadeOut(getGameComponents(), !isGame);
  fadeOut(getFolderComponents(), isGame);
  return true;
}

std::vector<Component*> DetailedGameListView::getFolderComponents()
{
  std::vector<Component*> comps;
  for (auto* img: mFolderContent)
  {
    comps.push_back(img);
  }
  comps.push_back(&mFolderName);
  return comps;
}

std::vector<Component*> DetailedGameListView::getGameComponents(bool includeMainComponents)
{
  std::vector<Component*> comps = getMDValues();
  if (includeMainComponents)
  {
    comps.push_back(&mNoImage);
    comps.push_back(&mImage);
    comps.push_back(&mVideo);
    comps.push_back(&mDescription);
  }
  std::vector<TextComponent*> labels = getMDLabels();
  comps.insert(comps.end(), labels.begin(), labels.end());
  return comps;
}

std::vector<Component*> DetailedGameListView::getScrapedFolderComponents()
{
  std::vector<Component*> comps;
  comps.push_back(&mNoImage);
  comps.push_back(&mImage);
  comps.push_back(&mVideo);
  comps.push_back(&mDescription);
  return comps;
}

void DetailedGameListView::setFolderInfo(FolderData* folder)
{
  char strbuf[256];

  FileData::List games = folder->GetAllDisplayableItemsRecursively(false, folder->System().Excludes());
  snprintf(strbuf, 256, _N("%i GAME AVAILABLE", "%i GAMES AVAILABLE", (int) games.size()).c_str(), games.size());
  mFolderName.setText(folder->Name() + " - " + strbuf);

  unsigned char idx = 0;

  for (FileData* game : games)
  {
    if (game->HasThumbnailOrImage())
    {
      mFolderContent[idx]->setImage(game->ThumbnailOrImagePath());
      if (++idx == mFolderContent.size())
        break;
    }
  }
  for (int i = idx; i < (int) mFolderContent.size(); i++)
  {
    mFolderContent[i]->setImage(Path());
  }
  // Kill video on multi-thumbnail folder
  mVideo.setVideo(Path::Empty, 0, 0);
}

void DetailedGameListView::SetImageFading(FileData* game, bool update)
{
  // Setup
  mNoImage.setImage(Path(":/no_image.png"));
  mNoImage.setThemeDisabled(false);

  bool imageExists = game->Metadata().Image().Exists();
  if (game->IsFolder())
  {
    // Just set the image if it exists - if not, a folder preview is displayed
    mImage.setImage(game->Metadata().Image());
    // Let no image display if the image does not exists
    mNoImage.setThemeDisabled(imageExists);
    mNoImage.setOpacity(255);
  }
  else
  {
    // Check equality with previous image
    bool didntExist = !mImage.getImagePath().Exists();
    // Set new image
    mImage.setImage(imageExists ? game->Metadata().Image() : Path::Empty);
    // if updating from no image, let's fade
    if (update && imageExists && didntExist) // Start fading!
    {
      mFadeBetweenImage = 255;
      mImage.setOpacity(255 - mFadeBetweenImage);
      mNoImage.setOpacity(mFadeBetweenImage);
    }
    else // Just disable no image if the image exists
    {
      mNoImage.setThemeDisabled(imageExists);
      mNoImage.setOpacity(255);
    }
  }
}

void DetailedGameListView::setGameInfo(FileData* file, bool update)
{
  if(mSystem.Name() != "imageviewer")
    setRegions(file);

  mRating.setValue(file->Metadata().RatingAsString());
  mReleaseDate.setValue(file->Metadata().ReleaseDate());
  mDeveloper.setValue(file->Metadata().Developer().empty() ? _("UNKNOWN") : file->Metadata().Developer());
  mPublisher.setValue(file->Metadata().Publisher().empty() ? _("UNKNOWN") : file->Metadata().Publisher());
  mGenre.setValue(file->Metadata().Genre().empty() ? _("NONE") : file->Metadata().Genre());
  mPlayers.setValue(file->Metadata().PlayersAsString());
  mLastPlayed.setValue(file->Metadata().LastPlayed());
  mPlayCount.setValue(file->Metadata().PlayCountAsString());
  mFavorite.setValue(file->Metadata().Favorite() ? _("YES") : _("NO"));

  int videoDelay = (int) mSettings.AsUInt("emulationstation.videosnaps.delay", VideoComponent::DEFAULT_VIDEODELAY);
  int videoLoop  = (int) mSettings.AsUInt("emulationstation.videosnaps.loop", VideoComponent::DEFAULT_VIDEOLOOP);

  SetImageFading(file, update);

  mBusy.setPosition(mImage.getPosition());
  mBusy.setSize(mImage.getSize());
  mBusy.setOrigin(mImage.getOrigin());

  if (!mSettings.AsBool("system.secondminitft.enabled", false) ||
      !mSettings.AsBool("system.secondminitft.disablevideoines", false))
    mVideo.setVideo(file->Metadata().Video(), videoDelay, videoLoop, AudioModeTools::CanDecodeVideoSound());

  { LOG(LogDebug) << "[GamelistView] Set video '" << file->Metadata().Video().ToString() << "' for " << file->Metadata().Name() << " => " << file->FilePath().ToString(); }

  mDescription.setText(file->Metadata().Description());
  mDescContainer.reset();
}

void DetailedGameListView::setScrapedFolderInfo(FileData* file)
{
  SetImageFading(file, false);
  mVideo.setVideo(Path::Empty, 0, 0);
  mDescription.setText(file->Metadata().Description());
  mDescContainer.reset();
}

void DetailedGameListView::fadeOut(const std::vector<Component*>& comps, bool fadingOut)
{
  for (auto* comp : comps)
  {
    // an animation is playing
    //   then animate if reverse != fadingOut
    // an animation is not playing
    //   then animate if opacity != our target opacity
    if ((comp->isAnimationPlaying(0) && comp->isAnimationReversed(0) != fadingOut) ||
        (!comp->isAnimationPlaying(0) && comp->getOpacity() != (fadingOut ? 0 : 255)))
    {
      auto func = [comp](float t)
      {
        comp->setOpacity((unsigned char) (lerp<float>(0.0f, 1.0f, t) * 255));
      };
      comp->setAnimation(new LambdaAnimation(func, 150), 0, nullptr, fadingOut);
    }
  }
}

void DetailedGameListView::launch(FileData* game)
{
  VideoEngine::Instance().StopVideo(true);
  mVideo.setVideo(Path::Empty, 0, 0);

  Vector3f target(Renderer::Instance().DisplayWidthAsFloat() / 2.0f, Renderer::Instance().DisplayHeightAsFloat() / 2.0f, 0);
  if (mImage.hasImage())
    target.Set(mImage.getCenter().x(), mImage.getCenter().y(), 0);

  ViewController::Instance().Launch(game, GameLinkedData(), target);
}

// element order need to follow the one in onThemeChanged
std::vector<TextComponent*> DetailedGameListView::getMDLabels()
{
  std::vector<TextComponent*> ret;
  ret.push_back(&mLblRating);
  ret.push_back(&mLblReleaseDate);
  ret.push_back(&mLblDeveloper);
  ret.push_back(&mLblPublisher);
  ret.push_back(&mLblGenre);
  ret.push_back(&mLblPlayers);
  ret.push_back(&mLblLastPlayed);
  ret.push_back(&mLblPlayCount);
  if (mSystem.HasFavoritesInTheme())
  {
    ret.push_back(&mLblFavorite);
  }
  return ret;
}

// element order need to follow the one in onThemeChanged
std::vector<Component*> DetailedGameListView::getMDValues()
{
  std::vector<Component*> ret;
  ret.push_back(&mRating);
  ret.push_back(&mReleaseDate);
  ret.push_back(&mDeveloper);
  ret.push_back(&mPublisher);
  ret.push_back(&mGenre);
  ret.push_back(&mPlayers);
  ret.push_back(&mLastPlayed);
  ret.push_back(&mPlayCount);
  if (mSystem.HasFavoritesInTheme())
  {
    ret.push_back(&mFavorite);
  }
  return ret;
}

void DetailedGameListView::Update(int deltatime)
{
  BasicGameListView::Update(deltatime);

  mBusy.Enable(mIsScraping);
  mBusy.Update(deltatime);

  if (mFadeBetweenImage >= 0)
  {
    mFadeBetweenImage -= deltatime;
    int f = mFadeBetweenImage < 0 ? 0 : mFadeBetweenImage;
    mImage.setOpacity(255 - f);
    mNoImage.setOpacity(f);
  }

  // Cancel video
  if (mList.isScrolling())
    mVideo.setVideo(Path::Empty, 0, 0);
}

void DetailedGameListView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  renderChildren(trans);

  Renderer::SetMatrix(trans);
  //Renderer::DrawRectangle(mBusy.getPosition().x(), mBusy.getPosition().y(), mBusy.getSize().x(), mBusy.getSize().y(), 0x00000080);
  mBusy.Render(trans);
}


void DetailedGameListView::OverlayApply(const Vector2f& position, const Vector2f& size, FileData*& data, unsigned int& color)
{
  (void)position;
  (void)size;
  (void)data;
  (void)color;

  /*if (mHasGenre)
  {
    float height = size.y() - 2.0f;

    GameGenres genre = data->Metadata().GenreId();
    if (genre != GameGenres::None)
    {
      const Path& path = Genres::GetResourcePath(genre);
      Renderer::drawRect(position.x() + size.x() - 2 - height, position.y() + 1, height, height, color);
    }
  }*/
}

float DetailedGameListView::OverlayGetRightOffset()
{
  if (mHasGenre)
    return mList.EntryHeight() * 1.2f;

  return 0.0f;
}

DetailedGameListView::~DetailedGameListView()
{
  for(int i = (int)mFolderContent.size(); --i >= 0; )
    delete mFolderContent[i];
  mFolderContent.clear();

  for(int i = (int)mRegions.size(); --i >= 0; )
    delete mRegions[i];
  mRegions.clear();
}

void DetailedGameListView::setRegions(FileData* file)
{
  Strings::Vector regionList = Strings::SplitQuotted(file->Regions(), ',');

  // reinit non used region flags
  for(unsigned long idx = 0; idx < mRegions.size(); idx++)
  {
    if(regionList.size() <= idx)
      mRegions[idx]->setImage(Path());
  }

  if(regionList.empty())
  {
    mRegions[0]->setImage(Path());
    return;
  }

  int i = 0;
  for(auto& region: regionList)
  {
    if(mRegions[i]->getImagePath() != Path(":/regions/" + region + ".svg"))
      mRegions[i]->setImage(Path(":/regions/" + region + ".svg"));
    i++;
  }
}




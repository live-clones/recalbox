//
// Created by xizor on 20/05/18.
//

#include <RecalboxConf.h>
#include "GuiUpdateRecalbox.h"
#include "components/TextComponent.h"
#include "components/MenuComponent.h"
#include <utils/Files.h>
#include <utils/locale/LocaleHelper.h>
#include "utils/network/Http.h"
#include <MainRunner.h>

#define BUTTON_GRID_VERT_PADDING Renderer::Instance().DisplayHeightAsFloat() * 0.025f
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

GuiUpdateRecalbox::GuiUpdateRecalbox(WindowManager& window, const std::string& imageUrl, const std::string& sha1Url, const std::string& newVersion)
  : Gui(window)
  , mImageUrl(imageUrl)
  , mSha1Url(sha1Url)
  , mNewVersion(newVersion)
  , mTotalSize(0)
  , mCurrentSize(0)
  , mSender(this)
  , mBackground(window, Path(":/frame.png"))
  , mGrid(window, Vector2i(3, 4))
{
  mRebootIn = _("REBOOT IN %s");
  mError = _("Error downloading Recalbox %s... Please retry later!");
  Strings::ReplaceAllIn(mError, "%s", mNewVersion);

  addChild(&mBackground);
  addChild(&mGrid);

  std::shared_ptr<MenuTheme> menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  const float height = Renderer::Instance().DisplayHeightAsFloat() * (Renderer::Instance().IsSmallResolution() ? 0.7f : 0.5f);
  const float width = Renderer::Instance().DisplayWidthAsFloat() * (Renderer::Instance().IsSmallResolution() ? 0.8f : 0.6f);

  // Title
  mTitle = std::make_shared<TextComponent>(mWindow, _("DOWNLOADING UPDATE..."), menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);
  mGrid.setEntry(mTitle, Vector2i(1, 0), false, false, Vector2i(1,1) );

  // Text
  std::string text = _("We're downloading Recalbox version %s!\n\nOnce the download is complete, Recalbox will reboot and start installing the new version.\nTypical installations take about 5-10mn. DO NOT reboot or power off Recalbox until the installation is complete.");
  Strings::ReplaceAllIn(text, "%s", newVersion);
  mText = std::make_shared<TextComponent>(mWindow, text, menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mGrid.setEntry(mText, Vector2i(1, 1), false, false, Vector2i(1,1) );

  // Progress bar
  mBar = std::make_shared<ProgressBarComponent>(mWindow, 1);
  mGrid.setEntry(mBar, Vector2i(1, 2), false, true, Vector2i(1,1) );

  // ETA
  mEta = std::make_shared<TextComponent>(mWindow, _("START DOWNLOADING..."), menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mGrid.setEntry(mEta, Vector2i(1, 3), false, false, Vector2i(1,1) );

  // Background
  mBackground.setImagePath(menuTheme->menuBackground.path);
  mBackground.setCenterColor(menuTheme->menuBackground.color);
  mBackground.setEdgeColor(menuTheme->menuBackground.color);
  mBackground.fitTo({ width, height }, Vector3f::Zero(), Vector2f(-32, -32));

  mGrid.setColWidthPerc(0, 0.02f);
  mGrid.setColWidthPerc(1, 0.96f);
  mGrid.setColWidthPerc(2, 0.02f);

  // Set grid size
  float titlePercent = (mTitle->getFont()->getLetterHeight() * 2.6f) / height;
  float pbPercent = 0.08f;
  float etaPercent = (mEta->getFont()->getLetterHeight() * 3.6f) / height;
  float textPercent = 1.0f - (titlePercent + pbPercent + etaPercent);
  constexpr float mainColWidth = 0.96f;
  constexpr float marginWidth = (1.0f - mainColWidth) / 2;

  mGrid.setColWidthPerc(0, marginWidth, false);
  mGrid.setColWidthPerc(1, mainColWidth, false);
  mGrid.setColWidthPerc(2, marginWidth, false);
  mGrid.setRowHeightPerc(0, titlePercent, false);
  mGrid.setRowHeightPerc(1, textPercent, false);
  mGrid.setRowHeightPerc(2, pbPercent, false);
  mGrid.setRowHeightPerc(3, etaPercent, false);

  mText->setSize(width * mainColWidth, 0);

  mGrid.setSize(width, height);

  // Window
  setSize(width, height);
  setPosition((Renderer::Instance().DisplayWidthAsFloat() - width) / 2,
              (Renderer::Instance().DisplayHeightAsFloat() - height) / 2);

  // Check free bytes on share partition
  if (RecalboxSystem::isFreeSpaceLimit())
  {
    std::string message = _("You must have at least %dGB free on 'SHARE' partition!");
    Strings::ReplaceAllIn(message, "%d", Strings::ToString(RecalboxSystem::GetMinimumFreeSpaceOnSharePartition() >> 30));
    mWindow.displayMessage(message);
    Close();
    return;
  }

  // Avoid sleeping!
  mIsProcessing = true;

  // start the thread if not aleady done
  Thread::Start("DLUpdate");
}

GuiUpdateRecalbox::~GuiUpdateRecalbox()
{
  Thread::Stop();
}

bool GuiUpdateRecalbox::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    mRequest.Cancel();
    Close();
  }
  return Component::ProcessInput(event);
}

bool GuiUpdateRecalbox::getHelpPrompts(Help& help)
{
  mGrid.getHelpPrompts(help);
  help.Set(Help::Cancel(), _("CANCEL"));
  return true;
}

void GuiUpdateRecalbox::Run()
{
  { LOG(LogDebug) << "[UpdateGui] Download update file"; }

  // Set boot partition R/W
  if (system("mount -o remount,rw /boot") != 0)
  {
    { LOG(LogError) << "[UpdateGui] Cannot mount /boot RW!"; }
    mSender.Call(-1);
  }
  // Empty target folder
  if (system(("rm -rf " + (Path(sDownloadFolder) / "*").ToString()).data()) != 0)
  { LOG(LogError) << "[UpdateGui] Cannot empty " << sDownloadFolder; }

  // Get arch
  std::string arch = Files::LoadFile(Path("/recalbox/recalbox.arch"));
  if (arch == "xu4") arch = "odroidxu4";

  // Get destination filename
  std::string destinationFileName = "recalbox-%.img.xz";
  Strings::ReplaceAllIn(destinationFileName, "%", arch);

  // Download
  Path destination = Path(sDownloadFolder) / destinationFileName;
  Path destinationSha1 = Path(sDownloadFolder) / destinationFileName.append(".sha1");
  { LOG(LogDebug) << "[UpdateGui] Target path " << destination.ToString(); }
  destination.Delete();
  mTimeReference = DateTime();
  mRequest.Execute(mImageUrl, destination, this);

  // Control & Reboot
  if (mTotalSize != 0)
    if (destination.Size() == mTotalSize)
    {
      // Download sha1
      mRequest.Execute(mSha1Url.append(".sha1"), destinationSha1, this);
      // Reboot
      MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot, false);
      return;
    }

  // Error
  destination.Delete();
  mSender.Call(-1);
}

void GuiUpdateRecalbox::DownloadProgress(const Http&, long long int currentSize, long long int expectedSize)
{
  // Store data and synchronize
  mTotalSize = expectedSize;
  mCurrentSize = currentSize;
  mSender.Call(0);
}

void GuiUpdateRecalbox::ReceiveSyncCallback(const SDL_Event& event)
{
  if (event.user.code == 0)
  {
    // Load size into progress bar component
    mBar->setMaxValue(mTotalSize);
    mBar->setCurrentValue(mCurrentSize);

    // Elapsed time
    if (mCurrentSize != 0)
    {
      TimeSpan elapsed = DateTime() - mTimeReference;
      TimeSpan eta((elapsed.TotalMilliseconds() * (mTotalSize - mCurrentSize)) / mCurrentSize);

      std::string text = mRebootIn;
      Strings::ReplaceAllIn(text, "%s", eta.ToTimeString());
      mEta->setText(text);
    }
  }
  else if (event.user.code < 0)
  {
    mEta->setText(mError);
    mGrid.onSizeChanged();
  }
}


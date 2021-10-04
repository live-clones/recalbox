//
// Created by bkg2k on 31/12/2019.
//

#include <themes/MenuThemeData.h>
#include <utils/locale/LocaleHelper.h>
#include <components/TextComponent.h>
#include <components/ButtonComponent.h>
#include <components/ScrollableContainer.h>
#include "components/MenuComponent.h" // for makeButtonGrid
#include <bios/BiosManager.h>
#include "GuiBiosScan.h"
#include "GuiBiosMd5.h"
#include <algorithm>
#include <systems/SystemManager.h>

static std::map<Bios::ReportStatus, const char*> sSuffixes(
{
  { Bios::ReportStatus::Unknown, " - \uF1C1" },
  { Bios::ReportStatus::Green  , " - \uF1C0" },
  { Bios::ReportStatus::Yellow , " - \uF1C0" },
  { Bios::ReportStatus::Red    , " - \uF1C2" },
});

GuiBiosScan::GuiBiosScan(WindowManager& window, SystemManager& systemManager)
  : Gui(window),
    mBackground(window, Path(":/frame.png")),
    mGrid(window, Vector2i(7,15)),
    mBusyAnim(window),
    mSystemManager(systemManager),
    mScanInProgress(false),
    mShowAllSystems(false)
{
  // Window's background
  addChild(&mBackground);
  // Add grid as the main component
  addChild(&mGrid);

  // Get theme
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mBackground.setImagePath(menuTheme->menuBackground.path);
  mBackground.setCenterColor(menuTheme->menuBackground.color);
  mBackground.setEdgeColor(menuTheme->menuBackground.color);

  // Bold condensed
  mBoldCondensed = Font::get(menuTheme->menuTextSmall.font->getSize(), Path("/usr/share/fonts/truetype/ubuntu_condensed.ttf"));

  // 7 x 15
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  // | |                                                         TITLE                                                                  | |  0 = 2%
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  // | |                                                         HEADER                                                                 | |  1 = 2%
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |      SYSTEM : | AMIGA THE BEST COMPUTOSAURUS                                 | |  2 = Font height
  // +-+                    LIST                         +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |                                     SPACER                                   | |  3 = 2%
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |       CORES : | Amiberry, libretro-puae                                      | |  4 = Font height
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |        PATH : | /recalbox/share/roms/amiga.bios.rom                          | |  5 = Font height
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |   MANDATORY : | YES/NO                 | MUST MATCH : | YES/NO               | |  6 = Font height
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |                                     SPACER                                   | |  7 = 2%
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |  FILE FOUND : | YES/NO                 |  MD5 MATCH : | YES/NO               | |  8 = Font height
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 | Details on 2 lines                                    |  STATUS              | |  9 = Font height * 4
  // | |                                                 |                                                       |  IMAGE               | |
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 |                                     SPACER                                   | | 10 = 2%
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 | NOTES :       |                                                              | | 11 = Font height
  // +-+                                                 +---------------+------------------------+--------------+----------------------+-+
  // | |                                                 | Special notes regarding this bios                                            | |
  // | |                                                 |                                                                              | | 12 = remaining space
  // | |                                                 |                                                                              | |
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  // | |                                                         FOOTER                                                                 | | 13 = Font height
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  // | |                                                         BUTTONS                                                                | | 14 = Button height
  // +-+-------------------------------------------------+---------------+------------------------+--------------+----------------------+-+
  //  0                        1                                 2                    3                   4                  5           6
  //  2%                      40%                               12%                  16%                 12%                16%          2%

  /*
   * Populate the grid
   */

  // Title
  mTitle = std::make_shared<TextComponent>(window, _("BIOS CHECKING"), menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);
  mGrid.setEntry(mTitle, Vector2i(1, 0), false, false, Vector2i(5,1) );

  // Header
  mHeader = std::make_shared<TextComponent>(window, "", menuTheme->menuFooter.font, menuTheme->menuFooter.color, TextAlignment::Center);
  mGrid.setEntry(mHeader, Vector2i(1, 1), false, true, Vector2i(5,1) );

  // List
  mList = std::make_shared<TextListComponent<ListContext>>(window);
  mList->setFont(menuTheme->menuTextSmall.font);
  mList->setAlignment(HorizontalAlignment::Right);
  mList->setSelectorColor(menuTheme->menuText.selectorColor);
  mList->setSelectedColor(menuTheme->menuText.selectedColor);
  mList->setColor(sColorIndexNormal, menuTheme->menuText.color);
  mList->setColor(sColorIndexHalf, (menuTheme->menuText.color & 0xFFFFFF00) | 0x80);
  mList->setColor(sColorIndexRed, 0xFF000022); // Red
  mList->setColor(sColorIndexYellow, 0xFFFF0022); // Yellow
  mList->setColor(sColorIndexGreen, 0x00FF0022); // Greeen
  mList->setHorizontalMargin(Renderer::Instance().DisplayWidthAsFloat() * 0.95f * 0.01f);
  mList->setSelectorHeight((float)menuTheme->menuTextSmall.font->getSize() * 1.5f);
  mList->setCursorChangedCallback([this](const CursorState& state) { (void)state; UpdateBiosDetail(); });
  mGrid.setEntry(mList, Vector2i(1,2), true, false, Vector2i(1, 11));

  // Details components
  mDetailSystemLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailSystemValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailCoreLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailCoreValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailPathLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailPathValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailMandatoryLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailMandatoryValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailHashMustMatchLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailHashMustMatchValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailFileFoundLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailFileFoundValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailHashIsMatchingLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailHashIsMatchingValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailText1Label = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailText1ValueContainer = std::make_shared<ScrollableContainer>(window);
  mDetailText1Value = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailText2Label = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailText2ValueContainer = std::make_shared<ScrollableContainer>(window);
  mDetailText2Value = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailText1ValueContainer->addChild(mDetailText1Value.get());
  mDetailText2ValueContainer->addChild(mDetailText2Value.get());
  mDetailStatusImage = std::make_shared<ImageComponent>(window);

  mDetailBiosOkLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosOkValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailBiosUnsafeLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosUnsafeValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailBiosKOLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosKOValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailBiosMatchingLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosMatchingValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailBiosNotMatchingLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosNotMatchingValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);
  mDetailBiosNotFoundLabel = std::make_shared<TextComponent>(window, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Right);
  mDetailBiosNotFoundValue = std::make_shared<TextComponent>(window, "", mBoldCondensed, menuTheme->menuTextSmall.color, TextAlignment::Left);

  mDetailSystemLabel->setUppercase(true);
  mDetailCoreLabel->setUppercase(true);
  mDetailPathLabel->setUppercase(true);
  mDetailMandatoryLabel->setUppercase(true);
  mDetailHashMustMatchLabel->setUppercase(true);
  mDetailFileFoundLabel->setUppercase(true);
  mDetailHashIsMatchingLabel->setUppercase(true);
  mDetailText1Label->setUppercase(true);
  mDetailText2Label->setUppercase(true);

  mGrid.setEntry(mDetailSystemLabel, Vector2i(2,2), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailSystemValue, Vector2i(3,2), false, false, Vector2i(3,1));
  mGrid.setEntry(mDetailCoreLabel, Vector2i(2,4), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailCoreValue, Vector2i(3,4), false, false, Vector2i(3,1));
  mGrid.setEntry(mDetailPathLabel, Vector2i(2,5), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailPathValue, Vector2i(3,5), false, false, Vector2i(3,1));
  mGrid.setEntry(mDetailMandatoryLabel, Vector2i(2,6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailMandatoryValue, Vector2i(3,6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailHashMustMatchLabel, Vector2i(4,6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailHashMustMatchValue, Vector2i(5,6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailFileFoundLabel, Vector2i(2,8), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailFileFoundValue, Vector2i(3,8), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailHashIsMatchingLabel, Vector2i(4, 8), false, false, Vector2i(1, 1));
  mGrid.setEntry(mDetailHashIsMatchingValue, Vector2i(5,8), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailText1Label, Vector2i(2,8), false, false, Vector2i(4,1));
  mGrid.setEntry(mDetailText1ValueContainer, Vector2i(3,9), false, false, Vector2i(3,1));
  mGrid.setEntry(mDetailText2Label, Vector2i(2,11), false, false, Vector2i(4,1));
  mGrid.setEntry(mDetailText2ValueContainer, Vector2i(2,12), false, false, Vector2i(4,1));
  mGrid.setEntry(mDetailStatusImage, Vector2i(2,9), false, false, Vector2i(1,1));

  mGrid.setEntry(mDetailBiosOkLabel, Vector2i(2, 4), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosUnsafeLabel, Vector2i(2, 5), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosKOLabel, Vector2i(2, 6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosOkValue, Vector2i(3, 4), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosUnsafeValue, Vector2i(3, 5), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosKOValue, Vector2i(3, 6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosMatchingLabel, Vector2i(4, 4), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosNotMatchingLabel, Vector2i(4, 5), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosNotFoundLabel, Vector2i(4, 6), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosMatchingValue, Vector2i(5, 4), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosNotMatchingValue, Vector2i(5, 5), false, false, Vector2i(1,1));
  mGrid.setEntry(mDetailBiosNotFoundValue, Vector2i(5, 6), false, false, Vector2i(1,1));

  // Footer
  mFooter = std::make_shared<TextComponent>(window, "", menuTheme->menuFooter.font, menuTheme->menuFooter.color, TextAlignment::Center);
  mGrid.setEntry(mFooter, Vector2i(1, 13), false, true, Vector2i(5,1) );

  // Buttons
  mButtonClose = std::make_shared<ButtonComponent>(mWindow, _("CLOSE"), _("CLOSE"), [this] { Close(); } );
  std::vector<std::shared_ptr<ButtonComponent>> buttons;
  buttons.push_back(mButtonClose);
  mButtonGrid = makeButtonGrid(mWindow, buttons);
  mGrid.setEntry(mButtonGrid, Vector2i(1, 14), true, false, Vector2i(5,1));

  // Set Window position/size
  setSize(Renderer::Instance().DisplayWidthAsFloat() * 0.95f, Renderer::Instance().DisplayHeightAsFloat() * 0.849f);
  setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

  UpdateBiosList();

  if (BiosManager::Instance().SystemCount() == 0)
  {
    mWindow.displayMessage("BIOS DATABASE NOT FOUND OR DAMAGED!\n\nRestore or reinstall your Recalbox!");
    Close();
  }
}

void GuiBiosScan::onSizeChanged()
{
  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  // Main grid
  float titlePercent = mTitle->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float headerPercent = mHeader->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float footerPercent = mFooter->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float buttonPercent = (mButtonGrid->getSize().y() * 1.2f) / mSize.y();

  float spacerPercent = Renderer::Instance().IsSmallResolution() ? 0.0f : 0.02f;
  float systemPercent = mDetailSystemLabel->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float corePercent = mDetailCoreLabel->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float pathPercent = Renderer::Instance().IsSmallResolution() ? 0.01f : mDetailPathLabel->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float mandatoryPercent = mDetailMandatoryLabel->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float filePercent = mDetailFileFoundLabel->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float remarkPercent = mDetailText1Value->getFont()->getLetterHeight() * 8.6f / mSize.y();
  float notePercent = mDetailText2Label->getFont()->getLetterHeight() * 2.6f / mSize.y();
  float remainingPercent = 1.0f - (titlePercent + headerPercent + spacerPercent * 3 + systemPercent + corePercent + pathPercent + mandatoryPercent + filePercent + remarkPercent + notePercent + footerPercent + buttonPercent);
  if (remainingPercent < 0.0f) remainingPercent = 0.0f;

  mGrid.setRowHeightPerc(0, titlePercent, false);
  mGrid.setRowHeightPerc(1, headerPercent, false);
  mGrid.setRowHeightPerc(2, systemPercent, false);
  mGrid.setRowHeightPerc(3, spacerPercent, false);
  mGrid.setRowHeightPerc(4, corePercent, false);
  mGrid.setRowHeightPerc(5, pathPercent, false);
  mGrid.setRowHeightPerc(6, mandatoryPercent, false);
  mGrid.setRowHeightPerc(7, spacerPercent, false);
  mGrid.setRowHeightPerc(8, filePercent, false);
  mGrid.setRowHeightPerc(9, remarkPercent, false);
  mGrid.setRowHeightPerc(10, spacerPercent, false);
  mGrid.setRowHeightPerc(11, notePercent, false);
  mGrid.setRowHeightPerc(12, remainingPercent, false);
  mGrid.setRowHeightPerc(13, footerPercent, false);
  mGrid.setRowHeightPerc(14, buttonPercent, false);
  mGrid.setColWidthPerc(0, 0.02f, false);
  mGrid.setColWidthPerc(1, 0.40f, false);
  mGrid.setColWidthPerc(2, 0.16f, false);
  mGrid.setColWidthPerc(3, 0.10f, false);
  mGrid.setColWidthPerc(4, 0.20f, false);
  mGrid.setColWidthPerc(5, 0.10f, false);
  mGrid.setColWidthPerc(6, 0.02f, false);
  mGrid.setSize(mSize);

  // Resize all components
  float marginPercent = 0.95f;
  mDetailSystemLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(2));
  mDetailSystemValue->setSize(marginPercent * mGrid.getColWidth(3, 5), mGrid.getRowHeight(2));
  mDetailCoreLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(4));
  mDetailCoreValue->setSize(marginPercent * mGrid.getColWidth(3, 5), mGrid.getRowHeight(4));
  mDetailPathLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(5));
  mDetailPathValue->setSize(marginPercent * mGrid.getColWidth(3, 5), mGrid.getRowHeight(5));
  mDetailMandatoryLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(6));
  mDetailMandatoryValue->setSize(marginPercent * mGrid.getColWidth(3), mGrid.getRowHeight(6));
  mDetailHashMustMatchLabel->setSize(marginPercent * mGrid.getColWidth(4), mGrid.getRowHeight(6));
  mDetailHashMustMatchValue->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(6));
  mDetailFileFoundLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(8));
  mDetailFileFoundValue->setSize(marginPercent * mGrid.getColWidth(3), mGrid.getRowHeight(8));
  mDetailHashIsMatchingLabel->setSize(marginPercent * mGrid.getColWidth(4), mGrid.getRowHeight(8));
  mDetailHashIsMatchingValue->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(8));
  mDetailText1Label->setSize(marginPercent * mGrid.getColWidth(2, 5), mGrid.getRowHeight(8));
  mDetailText1ValueContainer->setSize(marginPercent * mGrid.getColWidth(3, 5), mGrid.getRowHeight(9));
  mDetailText2Label->setSize(marginPercent * mGrid.getColWidth(2,5), mGrid.getRowHeight(11));
  mDetailText2ValueContainer->setSize(marginPercent * mGrid.getColWidth(2, 5), mGrid.getRowHeight(12));
  mDetailText1Value->setSize(mDetailText1ValueContainer->getSize().x(), 0); // make desc text wrap at edge of container
  mDetailText2Value->setSize(mDetailText2ValueContainer->getSize().x(), 0); // make desc text wrap at edge of container
  mDetailStatusImage->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(9));
  mDetailStatusImage->setMaxSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(9));

  mDetailBiosOkLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(4));
  mDetailBiosUnsafeLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(5));
  mDetailBiosKOLabel->setSize(marginPercent * mGrid.getColWidth(2), mGrid.getRowHeight(6));
  mDetailBiosOkValue->setSize(marginPercent * mGrid.getColWidth(3), mGrid.getRowHeight(4));
  mDetailBiosUnsafeValue->setSize(marginPercent * mGrid.getColWidth(3), mGrid.getRowHeight(5));
  mDetailBiosKOValue->setSize(marginPercent * mGrid.getColWidth(3), mGrid.getRowHeight(6));
  mDetailBiosMatchingLabel->setSize(marginPercent * mGrid.getColWidth(4), mGrid.getRowHeight(4));
  mDetailBiosNotMatchingLabel->setSize(marginPercent * mGrid.getColWidth(4), mGrid.getRowHeight(5));
  mDetailBiosNotFoundLabel->setSize(marginPercent * mGrid.getColWidth(4), mGrid.getRowHeight(6));
  mDetailBiosMatchingValue->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(4));
  mDetailBiosNotMatchingValue->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(5));
  mDetailBiosNotFoundValue->setSize(marginPercent * mGrid.getColWidth(5), mGrid.getRowHeight(6));

  // List need to be anchored to the grid manually
  mList->setPosition(mGrid.getColWidth(0), mGrid.getRowHeight(0, 1));
  mList->setSize(mGrid.getColWidth(1), mGrid.getRowHeight(2, 12));

  mBusyAnim.setSize(mSize);
}

bool GuiBiosScan::ProcessInput(const InputCompactEvent& event)
{
  if (event.StartPressed())
  {
    StartRescan();
    return true;
  }

  if (event.CancelPressed())
  {
    Close();
    return true;
  }

  if (event.XPressed())
  {
    mShowAllSystems = !mShowAllSystems;
    updateHelpPrompts();
    UpdateBiosList();
    return true;
  }

  if (event.ValidPressed() && (mList->size() > 0))
  {
    const ListContext& context = mList->getSelected();
    if (context.mBios != nullptr)
      mWindow.pushGui(new GuiBiosMd5(mWindow, *context.mBios));
    else
      mWindow.pushGui(new GuiBiosMd5(mWindow, *context.mBiosList));
  }

  return Gui::ProcessInput(event);
}

void GuiBiosScan::Update(int deltaTime)
{
  Component::Update(deltaTime);

  if (mScanInProgress)
    mBusyAnim.Update(deltaTime);
}

void GuiBiosScan::Render(const Transform4x4f& parentTrans)
{
  Gui::Render(parentTrans);

  Transform4x4f trans = parentTrans * getTransform();
  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(mGrid.getPosition().x() + mGrid.getColWidth(0), mGrid.getPosition().y() + mGrid.getRowHeight(0, 1),
                          mGrid.getColWidth(1), mGrid.getRowHeight(2, 12), 0x00000011);
  Renderer::DrawRectangle(mGrid.getPosition().x() + mGrid.getColWidth(0,1), mGrid.getPosition().y() + mGrid.getRowHeight(0, 1),
                          mGrid.getColWidth(2,5), mGrid.getRowHeight(2, 12), 0x00000018);

  if (mScanInProgress)
    mBusyAnim.Render(trans);
}

void GuiBiosScan::UpdateBiosList()
{
  int previousCursorIndex = mList->isEmpty() ? -1 : mList->getCursorIndex();
  mList->clear();

  // Check
  const BiosManager& biosManager = BiosManager::Instance();
  if (biosManager.SystemCount() == 0)
  {
    mList->add(Strings::ToUpperUTF8(_("EMPTY LIST")), ListContext(), sColorIndexNormal, -1, HorizontalAlignment::Center);
    return;
  }

  // Statistics
  int fullWorkingSystem = 0;
  int workingSystem = 0;
  int nonWorkingSystem = 0;
  std::string lastNonWorkingSystemName;
  int totalBiosNotFound = 0;
  int totalBiosMatching = 0;
  int totalBiosNonMatching = 0;

  // Fill the system list
  for(int i = 0; i < biosManager.SystemCount(); ++i)
  {
    // Fill bios list
    const BiosList& biosList = biosManager.SystemBios(i);

    // Filtered?
    if (!mShowAllSystems)
      if (!IsSystemShown(biosList.Name()))
        continue;

    // Header
    SystemData* system = mSystemManager.SystemByName(biosList.Name());
    std::string prefix;
    if (system != nullptr) prefix = system->Descriptor().IconPrefix();
    int headerIndex = (int)mList->size();
    mList->add((!prefix.empty() ? prefix : "\uF200 ") + Strings::ToUpperUTF8(biosList.FullName()), ListContext(), sColorIndexNormal, -1, HorizontalAlignment::Left);
    ListContext headerContext = ListContext(&biosList, nullptr);

    totalBiosNotFound    += biosList.TotalFileNotFound();
    totalBiosMatching    += biosList.TotalHashMatching();
    totalBiosNonMatching += biosList.TotalHashNotMatching();

    // Bios
    for(int j = 0; j < biosList.BiosCount(); ++j)
    {
      const Bios& bios = biosList.BiosAt(j);
      mList->add(Strings::ToUpperASCII(bios.Filename()) + sSuffixes[bios.LightStatus()], ListContext(&biosList, &bios), bios.BiosStatus() == Bios::Status::FileNotFound ? sColorIndexHalf : sColorIndexNormal, -1, HorizontalAlignment::Right);
    }
    mList->setSelectedAt(headerIndex, headerContext);

    // Update system statistics
    switch(biosList.ReportStatus())
    {
      case Bios::ReportStatus::Green:
      {
        fullWorkingSystem++;
        mList->changeBackgroundColorAt(headerIndex, sColorIndexGreen);
        break;
      }
      case Bios::ReportStatus::Yellow:
      {
        workingSystem++;
        mList->changeBackgroundColorAt(headerIndex, sColorIndexYellow);
        break;
      }
      case Bios::ReportStatus::Unknown:
      case Bios::ReportStatus::Red:
      {
        nonWorkingSystem++;
        lastNonWorkingSystemName = biosList.FullName();
        mList->changeBackgroundColorAt(headerIndex, sColorIndexRed);
        break;
      }
    }
  }

  // Update header
  std::string headerText;
  if (nonWorkingSystem != 0)
  {
    headerText = nonWorkingSystem == 1 ?
                 Strings::Replace(_("EMULATOR %s MAY NOT WORK PROPERLY UNTIL ALL REQUIRED BIOS ARE MADE AVAILABLE!"), "%s", lastNonWorkingSystemName) :
      Strings::Replace(_("%i EMULATORS MAY NOT WORK PROPERLY UNTIL ALL REQUIRED BIOS ARE MADE AVAILABLE!"), "%i", Strings::ToString(nonWorkingSystem));
  }
  else headerText = _("CONGRATULATIONS! ALL EMULATORS SHOULD WORK PROPERLY!");
  mHeader->setValue(headerText);

  // Update footer
  std::string footerText;
  if (totalBiosNotFound != 0)
    footerText = Strings::Replace(_("%i BIOS NOT FOUND"), "%i", Strings::ToString(totalBiosNotFound));
  if (totalBiosNonMatching != 0)
    footerText.append((footerText.empty() ? "" : " - ") + Strings::Replace(_("%i NON-MATCHING BIOS FOUND"), "%i", Strings::ToString(totalBiosNonMatching)));
  if (totalBiosMatching != 0)
    footerText.append((footerText.empty() ? "" : " - ") + Strings::Replace(_("%i GOOD BIOS FOUND!"), "%i", Strings::ToString(totalBiosMatching)));
  mFooter->setValue(footerText);

  // Update bios details
  if ((unsigned int)previousCursorIndex < (unsigned int)mList->size())
    mList->setCursorIndex(previousCursorIndex);
  UpdateBiosDetail();

  // Force component size to refresh
  onSizeChanged();
}

void GuiBiosScan::UpdateBiosDetail()
{
  // Get context
  if (mList->isEmpty()) return;
  const ListContext& context = mList->getSelected();

  // Refresh bios & parent system?
  if (context.mBios != nullptr)
  {
    mDetailSystemLabel->setValue(_("SYSTEM") + " :");
    mDetailSystemValue->setValue(context.mBiosList->FullName());
    mDetailCoreLabel->setValue(_("Core") + " :");
    mDetailCoreValue->setValue(context.mBios->Cores());
    if (!Renderer::Instance().IsSmallResolution())
    {
      mDetailPathLabel->setValue(_("File Path") + " :");
      mDetailPathValue->setValue(context.mBios->Filepath().ToString());
    }
    mDetailMandatoryLabel->setValue(_("Mandatory") + " :");
    mDetailMandatoryValue->setValue(context.mBios->IsMandatory() ? _("YES") : _("NO"));
    mDetailHashMustMatchLabel->setValue(_("Must match MD5") + " :");
    mDetailHashMustMatchValue->setValue(context.mBios->IsHashMatchingMandatory() ? _("YES") : _("NO"));
    mDetailFileFoundLabel->setValue(_("File found ?") + " :");
    mDetailFileFoundValue->setValue(context.mBios->BiosStatus() == Bios::Status::FileNotFound ? _("NO") : _("YES"));
    mDetailHashIsMatchingLabel->setValue(_("Matching MD5 ?") + " :");
    mDetailHashIsMatchingValue->setValue(context.mBios->BiosStatus() == Bios::Status::HashMatching ? _("YES") : _("NO"));
    mDetailText1Label->setValue("");
    std::string explain;
    switch(context.mBios->BiosStatus())
    {
      case Bios::Status::Unknown: break;
      case Bios::Status::FileNotFound:
      {
        if (context.mBios->IsMandatory()) explain = _("This bios is required in order to run the emulator properly. You won't be able to run any games with the emulator/core above.");
        else explain = _("This bios is not mandatory in most cases. The emulator will run properly unless you want to use features requiring this particular bios file.");
        break;
      }
      case Bios::Status::HashNotMatching:
      {
        if (context.mBios->IsHashMatchingMandatory()) explain = _("Your bios file does not match any known MD5. However, there are chances of getting the emulator running fine.");
        else explain = _("Your bios file does not match any known MD5 but this is not required. So there are strong chances of getting everything running fine.");
        break;
      }
      case Bios::Status::HashMatching:
      {
        explain = _("Your bios file does match one of the known MD5. Everything will run fine!");
        break;
      }
    }
    mDetailText1Value->setValue(explain);
    if (!context.mBios->Notes().empty())
    {
      mDetailText2Label->setValue(_("NOTE") + " :");
      mDetailText2Value->setValue(context.mBios->Notes());
    }
    else
    {
      mDetailText2Label->setValue("");
      mDetailText2Value->setValue("");
    }
    const char* imagePath = ":/thumbs_up_yellow.svg";
    switch(context.mBios->LightStatus())
    {
      case Bios::ReportStatus::Unknown:
      case Bios::ReportStatus::Yellow: break;
      case Bios::ReportStatus::Green: imagePath = ":/thumbs_up_green.svg"; break;
      case Bios::ReportStatus::Red: imagePath = ":/thumbs_down_red.svg"; break;
    }
    mDetailStatusImage->setImage(Path(imagePath));
    mDetailBiosOkLabel->setValue("");
    mDetailBiosOkValue->setValue("");
    mDetailBiosUnsafeLabel->setValue("");
    mDetailBiosUnsafeValue->setValue("");
    mDetailBiosKOLabel->setValue("");
    mDetailBiosKOValue->setValue("");
    mDetailBiosNotFoundLabel->setValue("");
    mDetailBiosNotFoundValue->setValue("");
    mDetailBiosMatchingLabel->setValue("");
    mDetailBiosMatchingValue->setValue("");
    mDetailBiosNotMatchingLabel->setValue("");
    mDetailBiosNotMatchingValue->setValue("");
  }
  // ... or refresh only system?
  else if (context.mBiosList != nullptr)
  {
    mDetailSystemLabel->setValue(_("SYSTEM") + " :");
    mDetailSystemValue->setValue(context.mBiosList->FullName());
    mDetailCoreLabel->setValue("");
    mDetailCoreValue->setValue("");
    mDetailPathLabel->setValue("");
    mDetailPathValue->setValue("");
    mDetailMandatoryLabel->setValue("");
    mDetailMandatoryValue->setValue("");
    mDetailHashMustMatchLabel->setValue("");
    mDetailHashMustMatchValue->setValue("");
    mDetailFileFoundLabel->setValue("");
    mDetailFileFoundValue->setValue("");
    mDetailHashIsMatchingLabel->setValue("");
    mDetailHashIsMatchingValue->setValue("");
    mDetailText1Label->setValue("");
    mDetailText1Value->setValue("");
    mDetailText2Label->setValue("");
    mDetailText2Value->setValue("");
    const char* imagePath = ":/thumbs_up_green.svg";
    const char* text = "Congratulation, all cores/emulators listed below will work flawlessly!";
    if (context.mBiosList->TotalBiosKo() != 0)
    {
      imagePath = ":/thumbs_down_red.svg";
      text = "Cores/emulators listed below won't probably work until required bios are made available.";
    }
    else if (context.mBiosList->TotalBiosUnsafe() != 0)
    {
      imagePath = ":/thumbs_up_yellow.svg";
      text = "Cores/emulators listed below may work in most cases, unless you use features requiring special bios.";
    }
    mDetailStatusImage->setImage(Path(imagePath));
    mDetailBiosOkLabel->setValue(_("BIOS OK"));
    mDetailBiosOkValue->setValue(Strings::ToString(context.mBiosList->TotalBiosOk()));
    mDetailBiosUnsafeLabel->setValue(_("BIOS UNSAFE"));
    mDetailBiosUnsafeValue->setValue(Strings::ToString(context.mBiosList->TotalBiosUnsafe()));
    mDetailBiosKOLabel->setValue(_("BIOS KO"));
    mDetailBiosKOValue->setValue(Strings::ToString(context.mBiosList->TotalBiosKo()));
    mDetailBiosNotFoundLabel->setValue(_("BIOS NOT FOUND"));
    mDetailBiosNotFoundValue->setValue(Strings::ToString(context.mBiosList->TotalFileNotFound()));
    mDetailBiosMatchingLabel->setValue(_("MD5 OK"));
    mDetailBiosMatchingValue->setValue(Strings::ToString(context.mBiosList->TotalHashMatching()));
    mDetailBiosNotMatchingLabel->setValue(_("MD5 NOT OK"));
    mDetailBiosNotMatchingValue->setValue(Strings::ToString(context.mBiosList->TotalHashNotMatching()));
    mDetailText1Value->setValue(_S(std::string(text)));
    mDetailText2Label->setValue(_("Core") + " :");
    std::string cores = GetUniqueCoreList(*context.mBiosList);
    mDetailText2Value->setValue(cores);
  }
}

void GuiBiosScan::StartRescan()
{
  if (!mScanInProgress)
  {
    mScanInProgress = true;
    BiosManager::Instance().Scan(this);
  }
}

void GuiBiosScan::ScanProgress(const Bios& bios)
{
  // Refresh only the bios under the cursor. The whole list is refreshed when the scan ends
  for(int i = (int)mList->size(); --i >= 0; )
    if (mList->getObjects(i).mBios == &bios)
      mList->changeTextAt(i, Strings::ToUpperASCII(bios.Filename()) + sSuffixes[bios.LightStatus()]);
}

void GuiBiosScan::ScanComplete()
{
  UpdateBiosList();
  mScanInProgress = false;
}

bool GuiBiosScan::getHelpPrompts(Help& help)
{
  mGrid.getHelpPrompts(help);
  help.Set(HelpType::UpDown, _("SELECT"))
      .Set(HelpType::Start, _("RESCAN"))
      .Set(Help::Valid(), _("MD5 LIST"))
      .Set(Help::Cancel(), _("CLOSE"))
      .Set(HelpType::X, mShowAllSystems ? _("MY SYSTEMS") : _("ALL SYSTEMS"));
  return true;
}

std::string GuiBiosScan::GetUniqueCoreList(const BiosList& biosList)
{
  std::vector<std::string> list;
  for(int i = biosList.BiosCount(); --i >= 0; )
  {
    std::vector<std::string> subList = Strings::Split(biosList.BiosAt(i).Cores(), ',');
    for(const std::string& s : subList)
      if (std::find(list.begin(), list.end(), s) == list.end())
        list.push_back(s);
  }
  return Strings::Join(list, ", ");
}

bool GuiBiosScan::IsSystemShown(const std::string& systemNames)
{
  Strings::Vector list = Strings::Split(systemNames, ',');

  #ifdef DEBUG
  // Check
  bool check = false;
  for(const std::string& systemName : mSystemManager.GetDeclaredSystemShortNames())
    for(const std::string& s : list)
      if (systemName == s)
        check = true;
  if (!check) { LOG(LogError) << "[BiosGui] CHECK BIOS Systems: " << systemNames << " not found!"; }
  #endif

  for(const SystemData* systemData : mSystemManager.GetAllSystemList())
    for(const std::string& s : list)
      if (systemData->Name() == s)
        return true;

  return false;
}

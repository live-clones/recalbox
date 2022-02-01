//
// Created by bkg2k on 11/11/2020.
//

#include "Wizard.h"
#include <themes/MenuThemeData.h>
#include <utils/locale/LocaleHelper.h>
#include <components/TextComponent.h>
#include <components/ImageComponent.h>
#include <help/Help.h>

Wizard::Wizard(WindowManager& window, const std::string& title, int pageCount)
  : Gui(window)
  , mBackground(window)
  , mGrid(window, { 5, 3 })
  , mPageCount(pageCount)
  , mCurrentPage(-1)
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

  // 7 x 15
  // +-+-------------------------++--------------------------------------+-+
  // | |                                 TITLE                           | |  0 = Text size%
  // +-+-------------------------++--------------------------------------+-+
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |        IMAGE            ||                TEXT                  | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |  1 = remaining size
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // | |                         ||                                      | |
  // +-+-------------------------++--------------------------------------+-+
  // +-+-------------------------++--------------------------------------+-+  2 = 2% margin
  //  0           1               2                  3                    4
  //  2%          27%             2%                67%                   2%

  // Title
  mTitle = std::make_shared<TextComponent>(window, _S(title), menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);
  mGrid.setEntry(mTitle, Vector2i(1, 0), false, true, Vector2i(3,1) );

  // Image
  mImage = std::make_shared<ImageComponent>(window);
  mGrid.setEntry(mImage, Vector2i(1, 1), false, true, Vector2i(1,1) );

  // Text
  mText = std::make_shared<TextComponent>(window, "", menuTheme->menuText.font, menuTheme->menuText.color, TextAlignment::Center);
  mGrid.setEntry(mText, Vector2i(3, 1), false, true, Vector2i(1,1) );

  // Set Window position/size
  setSize(Renderer::Instance().DisplayWidthAsFloat() * 0.9f, Renderer::Instance().DisplayHeightAsFloat() * 0.8f);
  setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
  mGrid.setSize(mSize);

  // Sizes
  mGrid.setColWidthPerc(0, 0.02f, false);
  mGrid.setColWidthPerc(1, 0.27f, false);
  mGrid.setColWidthPerc(2, 0.02f, false);
  mGrid.setColWidthPerc(3, 0.67f, false);
  mGrid.setColWidthPerc(4, 0.02f, true);
  mGrid.setRowHeightPerc(0, mTitle->getFont()->getLetterHeight() * 2.6f / mSize.y(), false);
  mGrid.setRowHeightPerc(1, 0.0f, false);
  mGrid.setRowHeightPerc(2, 0.04f, true);
  mGrid.SetColumnHighlight(true, 1, 1);

  // Image size
  mImage->setMaxSize(0/*mGrid.getColWidth(1)*/, mGrid.getRowHeight(1));
}

bool Wizard::ProcessInput(const InputCompactEvent& event)
{
  switch(OnKeyReceived(mCurrentPage, event))
  {
    case Move::Backward: { SetPage(mCurrentPage - 1); return true; }
    case Move::Foreward: { SetPage(mCurrentPage + 1); return true; }
    case Move::Close: { Close(); return true; }
    case Move::None:
    default: break;
  }
  return Component::ProcessInput(event);
}

void Wizard::Update(int deltaTime)
{
  Component::Update(deltaTime);

  if (mCurrentPage < 0)
    SetPage(0);
}

void Wizard::SetPage(int page)
{
  mCurrentPage = Math::clampi(page, 0, mPageCount - 1);

  mImage->setImage(OnImageRequired(mCurrentPage));
  mText->setText(OnTextRequired(mCurrentPage));

  mGrid.onSizeChanged();
  updateHelpPrompts();
}

bool Wizard::getHelpPrompts(Help& help)
{
  help.Clear();
  if (mCurrentPage > 0) help.Set(Help::Cancel(), _("BACK"));

  OnHelpRequired(mCurrentPage, help);

  return true;
}

void Wizard::UpdatePage()
{
  SetPage(mCurrentPage);
}


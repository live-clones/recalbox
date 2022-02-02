#include <recalbox/RecalboxSystem.h>
#include "components/MenuComponent.h"
#include "components/ButtonComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "themes/MenuThemeData.h"

#define BUTTON_GRID_VERT_PADDING Math::max(Renderer::Instance().DisplayHeightAsFloat() * 0.008f, 2.0f)
#define BUTTON_GRID_HORIZ_PADDING Math::max(Renderer::Instance().DisplayWidthAsFloat() * 0.01f, 3.0f)

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

MenuComponent::MenuComponent(WindowManager&window, const std::string& title, const std::shared_ptr<Font>& titleFont)
  : Component(window)
  , mBackground(window)
  , mGrid(window, Vector2i(1, 4))
  , mTimeAccumulator(0)
{
  (void)titleFont;

	addChild(&mBackground);
	addChild(&mGrid);

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	// set up title
	mTitle = std::make_shared<TextComponent>(mWindow);
	mTitle->setHorizontalAlignment(TextAlignment::Center);

	setTitle(title, menuTheme->menuTitle.font);
	mTitle->setColor(menuTheme->menuTitle.color);

	mGrid.setEntry(mTitle, Vector2i(0, 0), false);

  auto headerGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(5, 1));
  headerGrid->setColWidthPerc(0, 0.02);
  headerGrid->setColWidthPerc(4, 0.02);

  int batteryCharge = 0;
  int batteryIcon = 0;
  if (RecalboxSystem::getSysBatteryInfo(batteryCharge, batteryIcon))
  {
    mBattery = std::make_shared<TextComponent>(mWindow);
    mBattery->setFont(menuTheme->menuText.font);
    if (batteryCharge <= 15) mBattery->setColor(0xFF0000FF);
    else                     mBattery->setColor(menuTheme->menuText.color);
    mBattery->setText(' ' + Strings::unicode2Chars(batteryIcon) + ' ' + Strings::ToString(batteryCharge) + '%');
    mBattery->setHorizontalAlignment(TextAlignment::Left);
    headerGrid->setEntry(mBattery, Vector2i(1, 0), false);
  }

  if (RecalboxConf::Instance().GetClock())
  {
    mDateTime = std::make_shared<DateTimeComponent>(mWindow);
    mDateTime->setDisplayMode(DateTimeComponent::Display::RealTime);
    mDateTime->setHorizontalAlignment(TextAlignment::Right);
    mDateTime->setFont(menuTheme->menuText.font);
    mDateTime->setColor(menuTheme->menuText.color);
    headerGrid->setEntry(mDateTime, Vector2i(3, 0), false);
  }

  mGrid.setEntry(headerGrid, Vector2i(0, 0), false);

  // set up list which will never change (externally, anyway)
  mList = std::make_shared<ComponentList>(mWindow);
  mGrid.setEntry(mList, Vector2i(0, 1), true);

  updateGrid();
  updateSize();

  mGrid.resetCursor();
}

void MenuComponent::Update(int deltaTime)
{
  // Refresh every 2s
  if ((mTimeAccumulator += deltaTime) > 2000)
  {
    int batteryCharge = 0;
    int batteryIcon = 0;
    if (RecalboxSystem::getSysBatteryInfo(batteryCharge, batteryIcon))
    {
      auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
      if (batteryCharge <= 15) mBattery->setColor(0xFF0000FF);
      else mBattery->setColor(menuTheme->menuText.color);
      mBattery->setText(' ' + Strings::unicode2Chars(batteryIcon) + ' ' + Strings::ToString(batteryCharge) + '%');
    }
    mTimeAccumulator -= 2000;
  }
  Component::Update(deltaTime);
}

bool MenuComponent::ProcessInput(const InputCompactEvent& event)
{
  if (Component::ProcessInput(event)) return true;

  if (event.AnyDownPressed())
  {
    mGrid.setCursorTo(mList);
    mList->setCursorIndex(0);
    return true;
  }
  else if (event.AnyUpPressed())
  {
    mList->setCursorIndex(mList->size() - 1);
    if(!mButtons.empty()) mGrid.moveCursor(Vector2i(0, 1));
    else                  mGrid.setCursorTo(mList);
    return true;
  }
  return false;
}


void MenuComponent::setTitle(const std::string& title, const std::shared_ptr<Font>& font)
{
    mTitle->setText(Strings::ToUpperUTF8(title));
    mTitle->setFont(font);
}

float MenuComponent::getButtonGridHeight() const
{
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
    return ((mButtonGrid ? mButtonGrid->getSize().y() : 0 /*menuTheme->menuText.font->getHeight()*/) + BUTTON_GRID_VERT_PADDING);
}

void MenuComponent::updateSize()
{
    float menuTheme = MenuThemeData::getInstance()->getCurrentTheme()->menuSize.height;
    const float maxHeight = Renderer::Instance().DisplayHeightAsFloat() * menuTheme;
    float height = TITLE_HEIGHT + mList->getTotalRowHeight() + getButtonGridHeight() + getFooterHeight() + 2;
    if(height > maxHeight)
    {
        height = TITLE_HEIGHT + getButtonGridHeight() + getFooterHeight() + 2;
        int i = 0;
        while(i < mList->size())
        {
            float rowHeight = mList->getRowHeight(i);
            if(height + rowHeight < maxHeight)
                height += rowHeight;
            else
                break;
            i++;
        }
    }

    float width = Math::min(Renderer::Instance().DisplayHeightAsFloat() * 1.2f, Renderer::Instance().DisplayWidthAsFloat() * 0.90f);
    setSize(width, height);
}

void MenuComponent::onSizeChanged()
{
    mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

    // update grid row/col sizes
    mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
    mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y());
    mGrid.setRowHeightPerc(3, getFooterHeight() / mSize.y());

    mGrid.setSize(mSize);
}

void MenuComponent::addButton(const std::string& name, const std::string& helpText, const std::function<void()>& callback)
{
    mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, Strings::ToUpperUTF8(name), helpText, callback));
    updateGrid();
    updateSize();
}

void MenuComponent::updateGrid()
{
  Vector2i buttonGridCursor = Vector2i(0, 0);
  if(mButtonGrid)
  {
    buttonGridCursor = mButtonGrid->getCursor();
    mGrid.removeEntry(mButtonGrid);
  }
  if(mFooter) mGrid.removeEntry(mFooter);

  mButtonGrid.reset();

  if(!mButtons.empty())
  {
      mButtonGrid = makeButtonGrid(mWindow, mButtons);
      mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);
     mButtonGrid->setCursor(buttonGridCursor);
  }
  if (mFooter) mGrid.setEntry(mFooter, Vector2i(0,3), false, false);
}

bool MenuComponent::getHelpPrompts(Help& help)
{
  return mGrid.getHelpPrompts(help);
}

void MenuComponent::setFooter(const std::string& label)
{
  if (mFooter) mGrid.removeEntry(mFooter);
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mFooter = std::make_shared<TextComponent>(mWindow, label, menuTheme->menuFooter.font, menuTheme->menuFooter.color);
  updateGrid();
  updateSize();
}

float MenuComponent::getFooterHeight() const
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  return ((mFooter ? mFooter->getSize().y() : 0) + BUTTON_GRID_VERT_PADDING);
}

std::shared_ptr<ComponentGrid> makeButtonGrid(WindowManager&window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons)
{
    std::shared_ptr<ComponentGrid> grid = std::make_shared<ComponentGrid>(window, Vector2i(buttons.size(), 2));

    float gridWidth = (float) BUTTON_GRID_HORIZ_PADDING * buttons.size(); // initialize to padding
    for (int i = 0; i < (int)buttons.size(); i++)
    {
        grid->setEntry(buttons[i], Vector2i(i, 0), true, false);
        gridWidth += buttons[i]->getSize().x();
    }
    for (int i = 0; i < (int)buttons.size(); i++)
    {
        grid->setColWidthPerc(i, (buttons[i]->getSize().x() + BUTTON_GRID_HORIZ_PADDING) / gridWidth);
    }
    
    grid->setSize(gridWidth, buttons[0]->getSize().y() + BUTTON_GRID_VERT_PADDING + 2);
    grid->setRowHeightPerc(1, 2 / grid->getSize().y()); // spacer row to deal with dropshadow to make buttons look centered

    return grid;
}

/**
 * Limitation: same number of button per line, same dimension per cell
 */

std::shared_ptr<ComponentGrid> makeMultiDimButtonGrid(WindowManager&window, const std::vector< std::vector< std::shared_ptr<ButtonComponent> > >& buttons, const float outerWidth, const float outerHeight)
{
    const int sizeX = (int) buttons[0].size();
    const int sizeY = (int) buttons.size();
    const float buttonHeight = buttons[0][0]->getSize().y();
    const float gridHeight = (buttonHeight + BUTTON_GRID_VERT_PADDING + 2) * (float)sizeY;

    float horizPadding = (float) BUTTON_GRID_HORIZ_PADDING;
    float gridWidth, buttonWidth;

    do {
        gridWidth = outerWidth - horizPadding; // to get centered because size * (button size + BUTTON_GRID_VERT_PADDING) let a half BUTTON_GRID_VERT_PADDING left / right marge
        buttonWidth = (gridWidth / (float)sizeX) - horizPadding;
        horizPadding -= 2;
    } while ((buttonWidth < 100) && (horizPadding > 2));


    std::shared_ptr<ComponentGrid> grid = std::make_shared<ComponentGrid>(window, Vector2i(sizeX, sizeY));

    grid->setSize(gridWidth, gridHeight < outerHeight ? gridHeight : outerHeight);

    for (int x = 0; x < sizeX; x++)
        grid->setColWidthPerc(x, (float) 1 / (float)sizeX);

    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            const std::shared_ptr<ButtonComponent>& button = buttons[y][x];
            button->setSize(buttonWidth, buttonHeight);
            grid->setEntry(button, Vector2i(x, y), true, false);
        }
    }

    return grid;
}

std::shared_ptr<ImageComponent> makeArrow(WindowManager&window)
{
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
    auto bracket = std::make_shared<ImageComponent>(window);
    bracket->setImage(menuTheme->iconSet.arrow);
    bracket->setColorShift(menuTheme->menuText.color);
    bracket->setResize(0, round(menuTheme->menuText.font->getLetterHeight()));
    
    return bracket;
}

void MenuComponent::SetDefaultButton(int index)
{
    if ((unsigned int)index < (unsigned int)mButtons.size())
        mButtonGrid->setCursorTo(mButtons[index]);
}

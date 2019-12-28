#include <utils/Strings.h>
#include "components/ScraperSearchComponent.h"

#include "components/TextComponent.h"
#include "components/ScrollableContainer.h"
#include "components/ImageComponent.h"
#include "components/RatingComponent.h"
#include "components/DateTimeComponent.h"
#include "components/AnimatedImageComponent.h"
#include "Settings.h"
#include "utils/Log.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "Locale.h"
#include "MenuThemeData.h"

ScraperSearchComponent::ScraperSearchComponent(Window& window)
  : Component(window),
    mGrid(window, Vector2i(4, 3)),
    mBusyAnim(window)
{
	addChild(&mGrid);
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mRunning = true;

	// left spacer (empty component, needed for borders)
	mGrid.setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 0), false, false, Vector2i(1, 3), Borders::Top | Borders::Bottom);

	// selected result name
	mResultName = std::make_shared<TextComponent>(mWindow, "RESULT NAME", menuTheme->menuText.font, menuTheme->menuText.color);

	// selected result thumbnail
	mResultThumbnail = std::make_shared<ImageComponent>(mWindow);
	mGrid.setEntry(mResultThumbnail, Vector2i(1, 1), false, false, Vector2i(1, 1));

	// selected result desc + container
	mDescContainer = std::make_shared<ScrollableContainer>(mWindow);
	mResultDesc = std::make_shared<TextComponent>(mWindow, "RESULT DESC", menuTheme->menuText.font, menuTheme->menuText.color);
	mDescContainer->addChild(mResultDesc.get());
	mDescContainer->setAutoScroll(true);
	
	// metadata
	auto font = menuTheme->menuTextSmall.font; // this gets replaced in onSizeChanged() so its just a placeholder
	const unsigned int mdColor = menuTheme->menuText.color;
	const unsigned int mdLblColor = menuTheme->menuText.color;
	mMD_Rating = std::make_shared<RatingComponent>(mWindow, menuTheme->menuText.color);
	mMD_ReleaseDate = std::make_shared<DateTimeComponent>(mWindow);
	mMD_ReleaseDate->setColor(mdColor);
	mMD_ReleaseDate->setHorizontalAlignment(TextAlignment::Left);
	mMD_Developer = std::make_shared<TextComponent>(mWindow, "", font, mdColor);
	mMD_Publisher = std::make_shared<TextComponent>(mWindow, "", font, mdColor);
	mMD_Genre = std::make_shared<TextComponent>(mWindow, "", font, mdColor);
	mMD_Players = std::make_shared<TextComponent>(mWindow, "", font, mdColor);

	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Rating") + ":"), font, mdLblColor), mMD_Rating, false));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Released") + ":"), font, mdLblColor), mMD_ReleaseDate));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(_("Developer") + ":"), font, mdLblColor), mMD_Developer));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(_("Publisher") + ":"), font, mdLblColor), mMD_Publisher));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(_("Genre") + ":"), font, mdLblColor), mMD_Genre));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Players") + ":"), font, mdLblColor), mMD_Players));

	mMD_Grid = std::make_shared<ComponentGrid>(mWindow, Vector2i(2, (int)mMD_Pairs.size()*2 - 1));
	int i = 0;
	for (auto& mMD_Pair : mMD_Pairs)
	{
		mMD_Grid->setEntry(mMD_Pair.first, Vector2i(0, i), false, true);
		mMD_Grid->setEntry(mMD_Pair.second, Vector2i(1, i), false, mMD_Pair.resize);
		i += 2;
	}

	mGrid.setEntry(mMD_Grid, Vector2i(2, 1), false, false);

	updateViewStyle();
}

void ScraperSearchComponent::onSizeChanged()
{
	mGrid.setSize(mSize);
	
	if(mSize.x() == 0 || mSize.y() == 0) return;

	// column widths
  mGrid.setColWidthPerc(0, 0.02f); // looks better when this is higher in auto mode

	mGrid.setColWidthPerc(1, 0.25f);
	mGrid.setColWidthPerc(2, 0.25f);
	
	// row heights
	mGrid.setRowHeightPerc(0, (mResultName->getFont()->getHeight() * 1.6f) / mGrid.getSize().y()); // result name

	mGrid.setRowHeightPerc(2, 0.2f);

	const float boxartCellScale = 0.9f;

	// limit thumbnail size using setMaxHeight - we do this instead of letting mGrid call setSize because it maintains the aspect ratio
	// we also pad a little so it doesn't rub up against the metadata labels
	mResultThumbnail->setMaxSize(mGrid.getColWidth(1) * boxartCellScale, mGrid.getRowHeight(1));

	// metadata
	resizeMetadata();
	
	mDescContainer->setSize(mGrid.getColWidth(1)*boxartCellScale + mGrid.getColWidth(2), mResultDesc->getFont()->getHeight() * 3);

	mResultDesc->setSize(mDescContainer->getSize().x(), 0); // make desc text wrap at edge of container

	mGrid.onSizeChanged();

	mBusyAnim.setSize(mSize);
}

void ScraperSearchComponent::resizeMetadata()
{
	mMD_Grid->setSize(mGrid.getColWidth(2), mGrid.getRowHeight(1));
	if(mMD_Grid->getSize().y() > mMD_Pairs.size())
	{
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		//const int fontHeight = (int)(mMD_Grid->getSize().y() / mMD_Pairs.size() * 0.8f);
		auto fontLbl = menuTheme->menuTextSmall.font;
		auto fontComp = menuTheme->menuTextSmall.font;

		// update label fonts
		float maxLblWidth = 0;
		for (auto& mMD_Pair : mMD_Pairs)
		{
			mMD_Pair.first->setFont(fontLbl);
			mMD_Pair.first->setSize(0, 0);
			if(mMD_Pair.first->getSize().x() > maxLblWidth)
				maxLblWidth = mMD_Pair.first->getSize().x() + 6;
		}

		for (int i = 0; i < (int)mMD_Pairs.size(); i++)
		{
			mMD_Grid->setRowHeightPerc(i*2, (fontLbl->getLetterHeight() + 2) / mMD_Grid->getSize().y());
		}

		// update component fonts
		mMD_ReleaseDate->setFont(fontComp);
		mMD_Developer->setFont(fontComp);
		mMD_Publisher->setFont(fontComp);
		mMD_Genre->setFont(fontComp);
		mMD_Players->setFont(fontComp);

		mMD_Grid->setColWidthPerc(0, maxLblWidth / mMD_Grid->getSize().x());

		// rating is manually sized
		mMD_Rating->setSize(mMD_Grid->getColWidth(1), fontLbl->getHeight() * 0.65f);
		mMD_Grid->onSizeChanged();

		// make result font follow label font
		mResultDesc->setFont(menuTheme->menuTextSmall.font);
	}
}

void ScraperSearchComponent::updateViewStyle()
{
	// unlink description and result list and result name
	mGrid.removeEntry(mResultName);
	mGrid.removeEntry(mResultDesc);

  // show name
  mGrid.setEntry(mResultName, Vector2i(1, 0), false, true, Vector2i(2, 1), Borders::Top);

  // need a border on the bottom left
  mGrid.setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 2), false, false, Vector2i(3, 1), Borders::Bottom);

  // show description on the right
  mGrid.setEntry(mDescContainer, Vector2i(3, 0), false, false, Vector2i(1, 3), Borders::Top | Borders::Bottom);
  mResultDesc->setSize(mDescContainer->getSize().x(), 0); // make desc text wrap at edge of container
}

void ScraperSearchComponent::UpdateInfoPane(const FileData* game)
{
	if (game != nullptr)
	{

		mResultName->setText(Strings::ToUpperUTF8(game->getName()));
		mResultDesc->setText(Strings::ToUpperUTF8(game->Metadata().Description()));
		mDescContainer->reset();

		// Image
		Path image = game->Metadata().Image().IsEmpty() ? game->Metadata().Thumbnail() : game->Metadata().Image();
    mResultThumbnail->setImage(image);

    // metadata
		mMD_Rating->setValue(Strings::ToUpperASCII(game->Metadata().RatingAsString()));
		mMD_ReleaseDate->setValue(Strings::ToUpperASCII(game->Metadata().ReleaseDateAsString()));
		mMD_Developer->setText(Strings::ToUpperUTF8(game->Metadata().Developer()));
		mMD_Publisher->setText(Strings::ToUpperUTF8(game->Metadata().Publisher()));
		mMD_Genre->setText(Strings::ToUpperUTF8(game->Metadata().Genre()));
		mMD_Players->setText(Strings::ToUpperASCII(game->Metadata().PlayersAsString()));
		mGrid.onSizeChanged();
	}
	else
	{
		mResultName->setText("");
		mResultDesc->setText("");

    // Image
    mResultThumbnail->setImage(Path());

    // metadata
		mMD_Rating->setValue("");
		mMD_ReleaseDate->setValue("");
		mMD_Developer->setText("");
		mMD_Publisher->setText("");
		mMD_Genre->setText("");
		mMD_Players->setText("");
	}
}

bool ScraperSearchComponent::ProcessInput(const InputCompactEvent& event)
{
	return Component::ProcessInput(event);
}

void ScraperSearchComponent::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	renderChildren(trans);

	if (mRunning)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

    mBusyAnim.Render(trans);
	}
}

void ScraperSearchComponent::Update(int deltaTime)
{
  Component::Update(deltaTime);

	if (mRunning)
    mBusyAnim.Update(deltaTime);
}

bool ScraperSearchComponent::getHelpPrompts(Help& help)
{
	mGrid.getHelpPrompts(help);
	return true;
}

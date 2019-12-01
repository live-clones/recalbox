#include <utils/Strings.h>
#include "components/ScraperSearchComponent.h"

#include "guis/GuiMsgBox.h"
#include "components/TextComponent.h"
#include "components/ScrollableContainer.h"
#include "components/ImageComponent.h"
#include "components/RatingComponent.h"
#include "components/DateTimeComponent.h"
#include "components/AnimatedImageComponent.h"
#include "components/ComponentList.h"
#include "HttpReq.h"
#include "Settings.h"
#include "utils/Log.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "Locale.h"
#include "MenuThemeData.h"

ScraperSearchComponent::ScraperSearchComponent(Window& window, SearchType type)
  : Component(window),
    mGrid(window, Vector2i(4, 3)),
    mSearchType(type),
    mBusyAnim(window)
{
	addChild(&mGrid);
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBlockAccept = false;

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
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Developer") + ":"), font, mdLblColor), mMD_Developer));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Publisher") + ":"), font, mdLblColor), mMD_Publisher));
	mMD_Pairs.push_back(MetaDataPair(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(_("Genre") + ":"), font, mdLblColor), mMD_Genre));
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

	// result list
	mResultList = std::make_shared<ComponentList>(mWindow);
	mResultList->setCursorChangedCallback([this](CursorState state) { if(state == CursorState::Stopped) updateInfoPane(); });

	updateViewStyle();
}

void ScraperSearchComponent::onSizeChanged()
{
	mGrid.setSize(mSize);
	
	if(mSize.x() == 0 || mSize.y() == 0)
		return;

	// column widths
	if(mSearchType == SearchType::AutoAcceptFirst)
		mGrid.setColWidthPerc(0, 0.02f); // looks better when this is higher in auto mode
	else
		mGrid.setColWidthPerc(0, 0.01f);
	
	mGrid.setColWidthPerc(1, 0.25f);
	mGrid.setColWidthPerc(2, 0.25f);
	
	// row heights
	if(mSearchType == SearchType::AutoAcceptFirst) // show name
		mGrid.setRowHeightPerc(0, (mResultName->getFont()->getHeight() * 1.6f) / mGrid.getSize().y()); // result name
	else
		mGrid.setRowHeightPerc(0, 0.0825f); // hide name but do padding

	if(mSearchType == SearchType::AutoAcceptFirst)
	{
		mGrid.setRowHeightPerc(2, 0.2f);
	}else{
		mGrid.setRowHeightPerc(1, 0.505f);
	}

	const float boxartCellScale = 0.9f;

	// limit thumbnail size using setMaxHeight - we do this instead of letting mGrid call setSize because it maintains the aspect ratio
	// we also pad a little so it doesn't rub up against the metadata labels
	mResultThumbnail->setMaxSize(mGrid.getColWidth(1) * boxartCellScale, mGrid.getRowHeight(1));

	// metadata
	resizeMetadata();
	
	if(mSearchType != SearchType::AutoAcceptFirst)
		mDescContainer->setSize(mGrid.getColWidth(1)*boxartCellScale + mGrid.getColWidth(2), mResultDesc->getFont()->getHeight() * 3);
	else
		mDescContainer->setSize(mGrid.getColWidth(3)*boxartCellScale, mResultDesc->getFont()->getHeight() * 8);
	
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
	mGrid.removeEntry(mResultList);

	// add them back depending on search type
	if(mSearchType == SearchType::AutoAcceptFirst)
	{
		// show name
		mGrid.setEntry(mResultName, Vector2i(1, 0), false, true, Vector2i(2, 1), Borders::Top);

		// need a border on the bottom left
		mGrid.setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 2), false, false, Vector2i(3, 1), Borders::Bottom);

		// show description on the right
		mGrid.setEntry(mDescContainer, Vector2i(3, 0), false, false, Vector2i(1, 3), Borders::Top | Borders::Bottom);
		mResultDesc->setSize(mDescContainer->getSize().x(), 0); // make desc text wrap at edge of container
	}else{
		// fake row where name would be
		mGrid.setEntry(std::make_shared<Component>(mWindow), Vector2i(1, 0), false, true, Vector2i(2, 1), Borders::Top);

		// show result list on the right
		mGrid.setEntry(mResultList, Vector2i(3, 0), true, true, Vector2i(1, 3), Borders::Left | Borders::Top | Borders::Bottom);

		// show description under image/info
		mGrid.setEntry(mDescContainer, Vector2i(1, 2), false, false, Vector2i(2, 1), Borders::Bottom);
		mResultDesc->setSize(mDescContainer->getSize().x(), 0); // make desc text wrap at edge of container
	}
}

void ScraperSearchComponent::search(const ScraperSearchParams& params)
{
	mResultList->clear();
	mScraperResults.clear();
	mThumbnailReq.reset();
	mMDResolveHandle.reset();
	updateInfoPane();

	mLastSearch = params;
	mSearchHandle = startScraperSearch(params);
}

void ScraperSearchComponent::stop()
{
	mThumbnailReq.reset();
	mSearchHandle.reset();
	mMDResolveHandle.reset();
	mBlockAccept = false;
}

void ScraperSearchComponent::onSearchDone(const std::vector<ScraperSearchResult>& results)
{
	mResultList->clear();

	mScraperResults = results;

	const int end = (int)results.size() > MAX_SCRAPER_RESULTS ? MAX_SCRAPER_RESULTS : (int)results.size(); // at max display 5

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	auto font = menuTheme->menuText.font;
	unsigned int color = menuTheme->menuText.color;
	if(end == 0)
	{
		ComponentListRow row;
		row.addElement(std::make_shared<TextComponent>(mWindow, _("NO GAMES FOUND - SKIP"), font, color), true);

		if(mSkipCallback)
			row.makeAcceptInputHandler(mSkipCallback);

		mResultList->addRow(row);
		mGrid.resetCursor();
	}else{
		ComponentListRow row;
		for (int i = 0; i < end; i++)
		{
			row.elements.clear();
			row.addElement(std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(results.at(i).mdl.Name()), font, color), true);
			row.makeAcceptInputHandler([this, i] { returnResult(mScraperResults.at(i)); });
			mResultList->addRow(row);
		}
		mGrid.resetCursor();
	}

	mBlockAccept = false;
	updateInfoPane();

	if(mSearchType == SearchType::AutoAcceptFirst)
	{
		if(mScraperResults.empty())
			mSkipCallback();
		else
			returnResult(mScraperResults.front());
	}else if(mSearchType == SearchType::AutoAcceptMatchingCRC)
	{
		// TODO
	}
}

void ScraperSearchComponent::onSearchError(const std::string& error)
{
	LOG(LogInfo) << "ScraperSearchComponent search error: " << error;
	mWindow.pushGui(new GuiMsgBox(mWindow, Strings::ToUpperASCII(error),
                                 _("RETRY"), std::bind(&ScraperSearchComponent::search, this, mLastSearch),
                                 _("SKIP"), mSkipCallback,
                                 _("CANCEL"), mCancelCallback));
}

int ScraperSearchComponent::getSelectedIndex()
{
	if(mScraperResults.empty() || mGrid.getSelectedComponent() != mResultList)
		return -1;

	return mResultList->getCursorId();
}

void ScraperSearchComponent::updateInfoPane()
{
	int i = getSelectedIndex();
	if(mSearchType == SearchType::AutoAcceptFirst && (!mScraperResults.empty()))
	{
		i = 0;
	}
	
	if(i != -1 && (int)mScraperResults.size() > i)
	{
		ScraperSearchResult& res = mScraperResults.at(i);
		mResultName->setText(Strings::ToUpperASCII(res.mdl.Name()));
		mResultDesc->setText(Strings::ToUpperASCII(res.mdl.Description()));
		mDescContainer->reset();

		mResultThumbnail->setImage(Path());
		const std::string& thumb = res.thumbnailUrl.empty() ? res.imageUrl : res.thumbnailUrl;
		if(!thumb.empty())
		{
			mThumbnailReq = std::unique_ptr<HttpReq>(new HttpReq(thumb));
		}else{
			mThumbnailReq.reset();
		}

		// metadata
		mMD_Rating->setValue(Strings::ToUpperASCII(res.mdl.RatingAsString()));
		mMD_ReleaseDate->setValue(Strings::ToUpperASCII(res.mdl.ReleaseDateAsString()));
		mMD_Developer->setText(Strings::ToUpperASCII(res.mdl.Developer()));
		mMD_Publisher->setText(Strings::ToUpperASCII(res.mdl.Publisher()));
		mMD_Genre->setText(Strings::ToUpperASCII(res.mdl.Genre()));
		mMD_Players->setText(Strings::ToUpperASCII(res.mdl.PlayersAsString()));
		mGrid.onSizeChanged();
	}else{
		mResultName->setText("");
		mResultDesc->setText("");
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
	if (event.BPressed())
	{
		if(mBlockAccept)
			return true;
	}

	return Component::ProcessInput(event);
}

void ScraperSearchComponent::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	renderChildren(trans);

	if(mBlockAccept)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);
		//Renderer::drawRect((int)mResultList->getPosition().x(), (int)mResultList->getPosition().y(),
		//	(int)mResultList->getSize().x(), (int)mResultList->getSize().y(), 0x00000011);

    mBusyAnim.Render(trans);
	}
}

void ScraperSearchComponent::returnResult(const ScraperSearchResult& result)
{
	mBlockAccept = true;

	// resolve metadata image before returning
	if(!result.imageUrl.empty())
	{
		mMDResolveHandle = resolveMetaDataAssets(result, mLastSearch);
		return;
	}

	mAcceptCallback(result);
}

void ScraperSearchComponent::Update(int deltaTime)
{
  Component::Update(deltaTime);

	if(mBlockAccept)
	{
    mBusyAnim.Update(deltaTime);
	}

	if(mThumbnailReq && mThumbnailReq->status() != HttpReq::Status::InProgress)
	{
		updateThumbnail();
	}

	if(mSearchHandle && mSearchHandle->status() != AsyncStatus::InProgress)
	{
		auto status = mSearchHandle->status();
		auto results = mSearchHandle->getResults();
		auto statusString = mSearchHandle->getStatusString();

		// we reset here because onSearchDone in auto mode can call mSkipCallback() which can call 
		// another search() which will set our mSearchHandle to something important
		mSearchHandle.reset();

		if(status == AsyncStatus::Done)
		{
			onSearchDone(results);
		}else if(status == AsyncStatus::Error)
		{
			onSearchError(statusString);
		}
	}

	if(mMDResolveHandle && mMDResolveHandle->status() != AsyncStatus::InProgress)
	{
		if(mMDResolveHandle->status() == AsyncStatus::Done)
		{
			ScraperSearchResult result = mMDResolveHandle->getResult();
			mMDResolveHandle.reset();

			// this might end in us being deleted, depending on mAcceptCallback - so make sure this is the last thing we do in update()
			returnResult(result);
		}else if(mMDResolveHandle->status() == AsyncStatus::Error)
		{
			onSearchError(mMDResolveHandle->getStatusString());
			mMDResolveHandle.reset();
		}
	}
}

void ScraperSearchComponent::updateThumbnail()
{
	if(mThumbnailReq && mThumbnailReq->status() == HttpReq::Status::Success)
	{
		std::string content = mThumbnailReq->getContent();
		mResultThumbnail->setImage(content.data(), content.length());
		mGrid.onSizeChanged(); // a hack to fix the thumbnail position since its size changed
	}else{
		LOG(LogWarning) << "thumbnail req failed: " << mThumbnailReq->getErrorMsg();
		mResultThumbnail->setImage(Path());
	}

	mThumbnailReq.reset();
}

void ScraperSearchComponent::openInputScreen(ScraperSearchParams& params)
{
	auto searchForFunc = [&](const std::string& name)
	{
		params.nameOverride = name;
		search(params);
	};

	bool openOSK = Settings::Instance().UseOSK();

	stop();
	if (openOSK) {
		mWindow.pushGui(new GuiTextEditPopupKeyboard(mWindow, _("SEARCH FOR"),
			// initial value is last search if there was one, otherwise the clean path name
			params.nameOverride.empty() ? params.game->getScrappableName() : params.nameOverride,
			searchForFunc, false, _("SEARCH")));
	}
	else {
		mWindow.pushGui(new GuiTextEditPopup(mWindow, _("SEARCH FOR"),
			// initial value is last search if there was one, otherwise the clean path name
			params.nameOverride.empty() ? params.game->getScrappableName() : params.nameOverride,
			searchForFunc, false, _("SEARCH")));
	}
}

bool ScraperSearchComponent::getHelpPrompts(Help& help)
{
	mGrid.getHelpPrompts(help);
	if (getSelectedIndex() != -1)
		help.Set(HelpType::B, _("ACCEPT RESULT"));

	return true;
}

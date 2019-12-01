#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "scrapers/Scraper.h"

#include <functional>

class GuiMetaDataEd : public Gui
{
public:
		GuiMetaDataEd(Window&window, MetadataDescriptor& md, ScraperSearchParams params,
		const std::string& header, std::function<void()> savedCallback, std::function<void()> deleteFunc, SystemData* system, bool main);
	
	bool ProcessInput(const InputCompactEvent& event) override;
	void onSizeChanged() override;
	bool getHelpPrompts(Help& help) override;

private:
	void save();
	void fetch();
	void fetchDone(const ScraperSearchResult& result);
	void close(bool closeAllWindows);

	NinePatchComponent mBackground;
	ComponentGrid mGrid;
	
	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSubtitle;
	std::shared_ptr<ComponentGrid> mHeaderGrid;
	std::shared_ptr<ComponentList> mList;
	std::shared_ptr<ComponentGrid> mButtons;

	ScraperSearchParams mScraperParams;

	std::vector< std::shared_ptr<Component> > mEditors;
	std::vector< const MetadataFieldDescriptor* > mMetaDataEditable;

	MetadataDescriptor& mMetaData;
	std::function<void()> mSavedCallback;
	std::function<void()> mDeleteFunc;
};

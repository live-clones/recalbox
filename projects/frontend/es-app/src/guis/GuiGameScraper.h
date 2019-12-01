#pragma once

#include "guis/Gui.h"
#include "components/ScraperSearchComponent.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"

class GuiGameScraper : public Gui
{
public:
	GuiGameScraper(Window&window, const ScraperSearchParams& params, const std::function<void(const ScraperSearchResult&)>& doneFunc);

	void onSizeChanged() override;

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	bool getHelpPrompts(Help& help) override;

private:
	bool mClose;
	void close();

	ComponentGrid mGrid;
	NinePatchComponent mBox;

	std::shared_ptr<TextComponent> mGameName;
	std::shared_ptr<TextComponent> mSystemName;
	std::shared_ptr<ScraperSearchComponent> mSearch;
	std::shared_ptr<ComponentGrid> mButtonGrid;

	ScraperSearchParams mSearchParams;

	//std::function<void()> mCancelFunc;
};

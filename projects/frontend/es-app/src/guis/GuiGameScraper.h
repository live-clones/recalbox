#pragma once

#include "GuiComponent.h"
#include "components/ScraperSearchComponent.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"

class GuiGameScraper : public GuiComponent
{
public:
	GuiGameScraper(Window* window, const ScraperSearchParams& params, std::function<void(const ScraperSearchResult&)> doneFunc);

	void onSizeChanged() override;

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	std::vector<HelpPrompt> getHelpPrompts() override;

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

	std::function<void()> mCancelFunc;
};

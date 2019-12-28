#pragma once

#include "components/base/Component.h"
#include "components/ComponentGrid.h"
#include "components/BusyComponent.h"
#include <functional>
#include <games/FileData.h>

class ComponentList;
class ImageComponent;
class RatingComponent;
class TextComponent;
class DateTimeComponent;
class ScrollableContainer;
class HttpReq;
class AnimatedImageComponent;

class ScraperSearchComponent : public Component
{
public:
	explicit ScraperSearchComponent(Window& window);

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	bool getHelpPrompts(Help& help) override;
	void onSizeChanged() override;	
  void onFocusGained() override { mGrid.onFocusGained(); }
  void onFocusLost() override { mGrid.onFocusLost(); }

  void UpdateInfoPane(const FileData* game);

  /*!
   * @brief Set running state - display or not busy animation
   * @param running True to show the busy animation
   */
  void SetRunning(bool running) { mRunning = running; }

private:
	void updateViewStyle();

	void resizeMetadata();

	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mResultName;
	std::shared_ptr<ScrollableContainer> mDescContainer;
	std::shared_ptr<TextComponent> mResultDesc;
	std::shared_ptr<ImageComponent> mResultThumbnail;

	std::shared_ptr<ComponentGrid> mMD_Grid;
	std::shared_ptr<RatingComponent> mMD_Rating;
	std::shared_ptr<DateTimeComponent> mMD_ReleaseDate;
	std::shared_ptr<TextComponent> mMD_Developer;
	std::shared_ptr<TextComponent> mMD_Publisher;
	std::shared_ptr<TextComponent> mMD_Genre;
	std::shared_ptr<TextComponent> mMD_Players;

	// label-component pair
	struct MetaDataPair
	{
		std::shared_ptr<TextComponent> first;
		std::shared_ptr<Component> second;
		bool resize;

		MetaDataPair(std::shared_ptr<TextComponent> f, std::shared_ptr<Component> s, bool r = true) : first(std::move(f)), second(std::move(s)), resize(r) {};
	};
	
	std::vector<MetaDataPair> mMD_Pairs;

	bool mRunning;
	BusyComponent mBusyAnim;
};

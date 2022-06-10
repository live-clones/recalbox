#pragma once

#include "components/base/Component.h"
#include "components/TextComponent.h"
#include "components/IList.h"
#include "resources/TextureResource.h"
#include "themes/ThemeExtras.h"

class SystemManager;
class SystemData;
class AnimatedImageComponent;

enum class CarouselType : unsigned int
{
	Horizontal    = 0,
	Vertical      = 1,
	VerticalWheel = 2
};

struct SystemViewData
{
	std::shared_ptr<Component> logo;
	std::shared_ptr<Component> logotext;
	std::shared_ptr<ThemeExtras> backgroundExtras;
};

struct SystemViewCarousel
{
	CarouselType type;
	Vector2f pos;
	Vector2f size;
	Vector2f origin;
	float logoScale;
	float logoRotation;
	Vector2f logoRotationOrigin;
  TextAlignment logoAlignment;
	unsigned int color;
	int maxLogoCount; // number of logos shown on the carousel
	Vector2f logoSize;
	float zIndex;
};

class SystemView : public IList<SystemViewData, SystemData*>
{
public:
	SystemView(WindowManager& window, SystemManager& systemManager);

	void onShow() override {	mShowing = true; }
	void onHide() override {	mShowing = false; }

    void goToSystem(SystemData* system, bool animate);

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	
	void onThemeChanged(const ThemeData& theme);

	bool getHelpPrompts(Help& help) override;
	void ApplyHelpStyle() override;
	void populate();
	void removeFavoriteSystem();
	void manageFavorite();
	void manageSystemsList();

  /*!
  * @brief Add a system to system view
  * @param System data
  */
  void addSystem(SystemData * it);

  void removeSystem(SystemData * it);
  
  SystemData& CurrentSystem() const { return *mCurrentSystem; }

  SystemData* Prev();
  void RemoveCurrentSystem();
  void Sort();
  void onCursorChanged(const CursorState& state) override;

  protected:

private:
	void getViewElements(const ThemeData& theme);
	void getDefaultElements();
	void getCarouselFromTheme(const ThemeElement* elem);
  
	void renderCarousel(const Transform4x4f& parentTrans);
	void renderExtras(const Transform4x4f& parentTrans, float lower, float upper);
	void renderInfoBar(const Transform4x4f& trans);
	void renderFade(const Transform4x4f& trans);

  //! SystemManager instance
	SystemManager& mSystemManager;

	SystemViewCarousel mCarousel;
	TextComponent mSystemInfo;

	// unit is list index
	float mCamOffset;
	float mExtrasCamOffset;
	float mExtrasFadeOpacity;
	SystemData* mCurrentSystem;
	bool mViewNeedsReload;
	bool mShowing;
	bool launchKodi;
};

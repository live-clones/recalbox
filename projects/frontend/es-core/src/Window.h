#pragma once

#include "GuiComponent.h"
#include <vector>
#include "resources/Font.h"
#include "InputManager.h"

class HelpComponent;
class ImageComponent;

class Window
{
public:
	class InfoPopup
	{
	public:
		virtual void render(const Eigen::Affine3f& parentTrans) = 0;
	};
	Window();
	~Window();

	void pushGui(GuiComponent* gui);
	void displayMessage(std::string message);
	void displayScrollMessage(std::string title, std::string message);
	void removeGui(GuiComponent* gui);
	GuiComponent* peekGui();
	void deleteAllGui();

	void textInput(const char* text);
	void input(InputConfig* config, Input input);
	void update(int deltaTime);
	void render();

    bool init(unsigned int width = 0, unsigned int height = 0, bool initRenderer = true);
	void deinit();

	void normalizeNextUpdate();

	inline bool isShowingPopup() const { return mGuiStack.size() > 1; }
	inline bool isSleeping() const { return mSleeping; }
	bool getAllowSleep();
	void setAllowSleep(bool sleep);
	
	void renderLoadingScreen();

	void renderHelpPromptsEarly(); // used to render HelpPrompts before a fade
	void setHelpPrompts(const std::vector<HelpPrompt>& prompts, const HelpStyle& style);

	void setInfoPopup(std::shared_ptr<InfoPopup> infoPopup) { mInfoPopup = infoPopup; }
	//inline void stopInfoPopup() { if (mInfoPopup) mInfoPopup->~InfoPopup(); };

	void renderShutdownScreen();
	
	void doWake();

private:
	void onSleep();
	void onWake();
	void renderWaitingScreen(const std::string& text);

	// Returns true if at least one component on the stack is processing
	bool isProcessing();
	void renderScreenSaver();

	bool KonamiCode(InputConfig* config, Input input, Window* window);

	HelpComponent* mHelp;
	ImageComponent* mBackgroundOverlay;
	std::shared_ptr<InfoPopup> mInfoPopup;

	std::vector<GuiComponent*> mGuiStack;
	std::vector<std::string> mMessages;
	std::vector<std::string> mScrollMessages;
	std::vector<std::string> mScrollTitle;

	std::vector< std::shared_ptr<Font> > mDefaultFonts;

	int mFrameTimeElapsed;
	int mFrameCountElapsed;
	int mAverageDeltaTime;

	std::unique_ptr<TextCache> mFrameDataText;

	bool mNormalizeNextUpdate;

	bool mAllowSleep;
	bool mSleeping;
	unsigned int mTimeSinceLastInput;

	bool mRenderedHelpPrompts;

	std::string mKonami = "uuddlrlrba";
	int mKonamiCount = 0;
	const std::vector<std::string> mInputVals = { "up", "down", "left", "right", "a", "b" };
};

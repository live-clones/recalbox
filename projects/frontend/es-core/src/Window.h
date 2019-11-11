#pragma once

#include "GuiComponent.h"
#include <vector>
#include "resources/Font.h"
#include "input/InputManager.h"

class HelpComponent;
class ImageComponent;

class Window
{
public:
	class InfoPopup
	{
	public:
		virtual void render(const Transform4x4f& parentTrans) = 0;
	};
	Window();
	~Window();

	void pushGui(GuiComponent* gui);
	void displayMessage(const std::string& message);
	void displayScrollMessage(const std::string& title, const std::string& message);
	void removeGui(GuiComponent* gui);
	GuiComponent* peekGui();
	void deleteAllGui();

	void textInput(const char* text);
	void ProcessInput(const InputCompactEvent& event);
	void update(int deltaTime);
	void render();

  bool init(unsigned int width = 0, unsigned int height = 0, bool initRenderer = true);
	static void deinit();

  void normalizeNextUpdate() { mNormalizeNextUpdate = true; }

	inline bool isShowingPopup() const { return mGuiStack.size() > 1; }
	inline bool isSleeping() const { return mSleeping; }
  bool getAllowSleep() { return mAllowSleep; }
  void setAllowSleep(bool sleep) { mAllowSleep = sleep; }

  void renderLoadingScreen();

	void renderHelpPromptsEarly(); // used to render HelpPrompts before a fade
	void setHelpPrompts(const std::vector<HelpPrompt>& prompts, const HelpStyle& style);

	void setInfoPopup(const std::shared_ptr<InfoPopup>& infoPopup) { mInfoPopup = infoPopup; }
	//inline void stopInfoPopup() { if (mInfoPopup) mInfoPopup->~InfoPopup(); };

	void renderShutdownScreen();
	
	void doWake();

private:
	static void onSleep();
	static void onWake();
	void renderWaitingScreen(const std::string& text);

	// Returns true if at least one component on the stack is processing
	bool isProcessing();
	static void renderScreenSaver();

	bool KonamiCode(InputDevice* config, InputEvent input, Window* window);

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

	static constexpr int sKonamiLength = 10;
	InputDevice::Entry mKonami[sKonamiLength] =
	{
    InputDevice::Entry::Up,
    InputDevice::Entry::Up,
    InputDevice::Entry::Down,
    InputDevice::Entry::Down,
    InputDevice::Entry::Left,
    InputDevice::Entry::Right,
    InputDevice::Entry::Left,
    InputDevice::Entry::Right,
    InputDevice::Entry::B,
    InputDevice::Entry::A,
  };
	int mKonamiCount = 0;
	const std::vector<InputDevice::Entry> mInputVals =
  {
    InputDevice::Entry::Up,
    InputDevice::Entry::Down,
    InputDevice::Entry::Left,
    InputDevice::Entry::Right,
    InputDevice::Entry::A,
    InputDevice::Entry::B,
  };

	/*!
	 * @brief Delete GUI pending for deletion
	 */
  void deleteClosePendingGui();
};

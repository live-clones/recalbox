#pragma once

#include <utils/storage/Stack.h>
#include <guis/Gui.h>
#include <components/HelpComponent.h>
#include <components/ImageComponent.h>
#include <resources/Font.h>
#include <input/InputManager.h>

class Window
{
  public:
    class InfoPopup
    {
      public:
        virtual void Render(const Transform4x4f& parentTrans) = 0;
    };

    /*!
     * @brief Default constructor
     */
    Window();

    /*!
     * @brief Default destructor
     */
    ~Window();

    void pushGui(Gui* gui);

    void displayMessage(const std::string& message);

    void displayScrollMessage(const std::string& title, const std::string& message);

    Gui* peekGui();

    void deleteAllGui();

    void textInput(const char* text);

    virtual bool ProcessInput(const InputCompactEvent& event);

    virtual void Update(int deltaTime);

    virtual void Render(Transform4x4f& transform);

    void RenderAll();

    bool Initialize(unsigned int width = 0, unsigned int height = 0, bool initRenderer = true);

    static void Finalize();

    void normalizeNextUpdate() { mNormalizeNextUpdate = true; }

    bool isSleeping() const { return mSleeping; }

    //void renderLoadingScreen();

    void renderHelpPromptsEarly(); // used to render HelpPrompts before a fade
    void UpdateHelp() { mHelp.UpdateHelps(); }

    void setInfoPopup(const std::shared_ptr<InfoPopup>& infoPopup) { mInfoPopup = infoPopup; }

    //void renderShutdownScreen();

    void doWake()
    {
      mTimeSinceLastInput = 0;
      mSleeping = false;
    }

    /*!
     * @brief Close all gui
     */
    void CloseAll();

    /*!
     * @brief Check if the window has
     * @return
     */
    bool HasGui() const { return !mGuiStack.Empty(); }

    /*!
     * @brief Check if the given UI is on top of the screen
     * @return True if the given UI is the first visible
     */
    virtual bool AmIOnTopOfScreen(const Gui* ui) const
    {
      if (!mGuiStack.Empty())
        if (mGuiStack.Peek() == ui) return true;
      return false;
    }

  protected:
    /*!
     * @brief Update the help system.
     * @return True if the update system has been set, false otherwise
     */
    virtual bool UpdateHelpSystem();

  private:
    //void renderWaitingScreen(const std::string& text);

    // Returns true if at least one component on the stack is processing
    bool isProcessing();

    static void renderScreenSaver();

    bool KonamiCode(InputDevice* config, InputEvent input, Window& window);

    HelpComponent mHelp;
    ImageComponent mBackgroundOverlay;
    std::shared_ptr<InfoPopup> mInfoPopup;

    Stack<Gui*> mGuiStack;
    Strings::Vector mMessages;
    Strings::Vector mScrollMessages;
    Strings::Vector mScrollTitle;

    std::vector<std::shared_ptr<Font> > mDefaultFonts;
    std::unique_ptr<TextCache> mFrameDataText;

    int mFrameTimeElapsed;
    int mFrameCountElapsed;
    int mAverageDeltaTime;
    unsigned int mTimeSinceLastInput;

    bool mNormalizeNextUpdate;
    bool mSleeping;
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

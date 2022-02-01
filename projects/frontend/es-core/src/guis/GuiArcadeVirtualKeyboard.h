#pragma once

#include <guis/Gui.h>
#include <resources/Font.h>
#include <resources/TextureResource.h>
#include <utils/Strings.h>
#include <utils/datetime/DateTime.h>
#include <guis/IGuiArcadeVirtualKeyboardInterface.h>
#include <Renderer.h>

class GuiArcadeVirtualKeyboard : public Gui
{
  private:
    //! Wheel information holder
    struct Wheel
    {
      //! Current wheel index
      int mIndex;

      //! Current angle in the wheel - MUST BE ALWAYS POSITIVE
      double mAngle;

      //! Number of char in the current wheel
      int mWheelCharCount;

      Wheel()
        : mIndex(0),
          mAngle(0.0f),
          mWheelCharCount(0)
      {
      }
    };

    /*!
     * @brief Simple rectangle structure
     */
    struct Rectangle
    {
      float x; //!< Left
      float y; //!< Top
      float w; //!< Width
      float h; //!< Height
    };

    enum Direction
    {
      Left,
      None,
      Right,
    };

    /*
     * Statics
     */

    //! Pi constant
    static constexpr double Pi = 3.1415926535;

    //! Wheel change animation in milliseconds
    static constexpr int sWheelChangeAnimationTimeMs = 300;

    //! Wheel dimming transparency / 255
    static constexpr int sWheelDimmingTransparency = 200;

    //! Wheel change animation in milliseconds (normal)
    static constexpr int sWheelRotateAnimationSlowTimeMs = 150;
    //! Wheel change animation in milliseconds (fast)
    static constexpr int sWheelRotateAnimationFastTimeMs = 80;

    //! Wheel change animation in milliseconds (normal)
    static constexpr int sMoveCursorAnimationFirstTimeMs = 200;
    //! Wheel change animation in milliseconds (normal)
    static constexpr int sMoveCursorAnimationSlowTimeMs = 100;
    //! Wheel change animation in milliseconds (fast)
    static constexpr int sMoveCursorAnimationFastTimeMs = 50;

    //! Available character sets
    static constexpr const wchar_t* sWheelAlphaSmalls    = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    static constexpr const wchar_t* sWheelAlphaCaps      = L"abcdefghijklmnopqrstuvwxyz ";
    static constexpr const wchar_t* sWheelAlphaDigits    = L"0123456789+-*/=.,;:!?\\";
    static constexpr const wchar_t* sWheelAlphaSymbols   = L"$€&#@%£§_|<>{}()[]'`\"ß";
    static constexpr const wchar_t* sWheelDiacriticSmall = L"áàåâäãéèêëíìîïýóòôöõøúùûüñç";
    static constexpr const wchar_t* sWheelDiacriticCaps  = L"ÁÀÅÂÄÃÉÈÊËÍÌÎÏÝÓÒÔÖÕØÚÙÛÜÑÇ";

    //! Wheel count (charset count)
    static constexpr const int sWheelCount = 6;

    //! Font ratio (in screen %)
    static constexpr const float sWheelFontRatio = 0.12f;
    static constexpr const float sTitleFontRatio = 0.045f;
    static constexpr const float sTextFontRatio = 0.085f;

    //! Colors
    static constexpr const unsigned int sWheelCharColorNormal = 0xFFFFFFFF;
    static constexpr const unsigned int sWheelCharColorSelected = 0xFF0000FF;
    static constexpr const unsigned int sEditableTextColor = 0xFFFFFFFF;
    static constexpr const unsigned int sTitleTextColor = 0xFFFF80FF;

    /*
     * Private fields
     */

    //! Abvailable wheels
    static const wchar_t* const sWheels[sWheelCount];

    //! Individual wheel's saved angle
    double mSavedAngles[sWheelCount];

    //! Title
    std::string mTitle;
    //! Current text
    std::vector<unsigned int> mText;

    //! Current Wheels
    Wheel mCurrentWheel;
    //! Previous Wheel
    Wheel mPreviousWheel;
    //! Wheel dimming transparency timer
    int mWheelDimmingColor;
    //! Wheel dimming state
    bool mWheelDimming;

    //! Shown?
    bool mShown;

    //! Current cursor position in character
    int mCursor;
    //! Selection length if any (only in keybaord input mode)
    int mSelectionLength;
    //! Offset in pixel of the typed text relative to the edit rectangle
    float mOffsetInPixel;

    //! Cursor moving on/off
    bool mCursorOn;
    //! Cursor fast move
    bool mCursorFast;
    //! Cursor direction
    Direction mCursorDirection;
    //! Cursor countdown
    int mCursorTimerMs;
    //! Curor blinking time reference
    DateTime mCursorBlinkReference;

    //! Wheel animation countdown in milliseconds
    int mWheelChangeAnimation;
    //! Next char animation countdown in milliseconds
    int mCharChangeAnimation;

    //! Editor rectangle + margin
    Rectangle mOuterEditor;
    //! Real editor rectangle
    Rectangle mInnerEditor;

    //! Rotation directrion
    Direction mMoveDirection;
    //! Move fast
    bool mMoveFast;
    //! Rotation on off
    bool mMoveOn;

    //! Validation callback
    IGuiArcadeVirtualKeyboardInterface* mValidateCallback;

    /*
     * UI Components
     */

    //! Big font to draw cirlce characters
    std::shared_ptr<Font> mWheelFont;
    //! Font used to draw Title text
    std::shared_ptr<Font> mTitleFont;
    //! Font used to draw text
    std::shared_ptr<Font> mTextFont;

    //! Title character caches
    std::unique_ptr<TextCache> mTitleCache;

    //! Hold the image under the selected char
    std::shared_ptr<TextureResource> mSelectedChar;

    /*
     * Private Methods
     */

    /*!
     * @brief Initialize a Wheel using the provided index
     * @param wheel Wheel to initialize
     * @param index Wheel index
     */
    void BuildWheel(Wheel& wheel, int index);

    /*!
     * @brief Get the char at the current angle of the provided wheel
     * @param wheel Wheel structure
     * @return Char index
     */
    static int GetCurrentCharIndex(const Wheel& wheel);

    /*!
     * @brief Move to a new wheel using delta
     * @param delta Use -1 or +1 to move into available wheels
     */
    void ChangeWheel(int delta);

    /*!
     * @brief Start animation to move to the previous/next char
     * @param wheel Wheel to move into
     * @param left True = left, False = right
     * @param on On/off
     */
    void ChangeChar(bool fast, bool left, bool on);

    /*!
     * @brief Get integer font size regarding the given ratio and the screen size
     * @param ratio font size expressed in screen ratio
     * @return absolute font size
     */
    static int GetFontSize(float ratio)
    {
      int iRatio = (int)(ratio * Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat()));
      int max = (int)(sWheelFontRatio * 800);
      if (iRatio > max)
        iRatio = max;
      return iRatio;
    }

    /*!
     * @brief Start rotating the wheel
     * @param left True = left, False = right
     * @param fast Normal or Fast rotation
     */
    void StartMoving(bool left, bool fast);

    /*!
     * @brief Stop moving
     */
    void StopMoving();

    /*!
     * @brief Start rotating the cursor
     * @param left True = left, False = right
     * @param fast Normal or Fast rotation
     */
    void StartMovingCursor(bool left, bool fast);

    /*!
     * @brief Stop moving cursor
     */
    void StopMovingCursor();

    /*!
     * @brief Start animation to move to the previous/next char
     * @param wheel Wheel to move into
     * @param left True = left, False = right
     * @param on On/off
     */
    void ChangeCursor(bool left, bool fast, bool on);

    /*!
     * @brief Blen d a color with another
     * @param from Source color
     * @param to Destination color
     * @param percent From source color at 0.0 to destination color at 1.0
     * @return Blended color
     */
    static unsigned int BlendColor(unsigned int from, unsigned int to, double ratio);

    /*!
     * @brief Render the selector under the selected char
     */
    void RenderSelector();

    /*!
     * @brief Draw a wheel
     * @param wheel Wheel to draw
     * @param raymultiplier multiply font size and
     */
    void RenderWheel(const Wheel& wheel, double raymultiplier);

    /*!
     * @brief Draw a single char of the wheel
     * @param wheel Wheel
     * @param centerx Wheel center X coordinate
     * @param centery Wheel center Y coordinate
     * @param charindex Char index to draw
     * @param ray Wheel ray
     * @param sizeratio Char size ratio
     * @param morphangle Morphing ratio for color and size
     */
    void RenderWheelChar(const Wheel& wheel, double centerx, double centery, int charindex, double xray, double yray, double sizeratio, double morphangle);

    /*!
     * @brief Render text box in the middle of the screen
     */
    void RenderTextBox();

    /*!
     * @brief Render the string and the cursor
     */
    void RenderEditedString();

    /*!
     * @brief Prepare title text rendering
     */
    void PrepareTitle();

    /*!
     * @brief Move edit cursor
     * @param timedelta elapsed time since last call
     */
    void MoveEditCursor(int deltatime);

    /*!
     * @brief Adjust cursor position and adjust the string offset
     * when the cursor pass beyond the visible area of the string
     * @param cursorextend cursor extension left/right to force more characters to be in visible area
     *
     */
    void AdjustCursor(int trycenter = 0);

    /*!
     * @brief Get current selected char from the current wheel
     * @return Selected char
     */
    unsigned int GetSelectedCharacter();

    /*!
     * @brief Add the current char to the string
     * @param unicode charr to add to the edit string
     */
    void AddCharacter(unsigned int unicode);

    /*!
     * @brief Cancel the editor
     */
    void CancelEditor();

    /*!
     * @brief Validate the editor
     */
    void ValidateEditor();

    /*!
     * @brief Delete the char at the cursoir position
     */
    void Delete();

    /*!
     * @brief Delete the char to the left
     */
    void Backspace();

    /*!
     * @brief Reset cursor time reference
     */
    void ResetCursorBlink();

  protected:
    /*!
     * @brief Receive system inputs instead of direct event
     * @param text Typed text
     */
    void textInput(const char* text) override;

    /*!
     * @brief Receive input events
     * @param event Compact event
     * @return True if the method has processed the event, false otherwise
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Update component
     * @param deltaTime Elapsed time since last update
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Called when it's time to render.  By default, just calls renderChildren(parentTrans * getTransform()).
     * You probably want to override this like so:
     * 1. Calculate the new transform that your control will draw at with Eigen::Affine3f t = parentTrans * getTransform().
     * 2. Set the renderer to use that new transform as the model matrix - Renderer::setMatrix(t);
     * 3. Draw your component.
     * 4. Tell your children to render, based on your component's transform - renderChildren(t).
     * @param parentTrans Global transform
     */
    void Render(const Transform4x4f& parentTrans) override;

    /*!
     * @brief Get help system configuration
     * @param help Help system to fill in with the configuration
     * @return True if the configuration has been set.
     */
    bool getHelpPrompts(Help& help) override;

    /*!
     * @brief This GUI is a transparent overlay
     * @return True
     */
    bool IsOverlay() const override { return true; }

    /*!
     * @brief If true, the help system is rendered under this gui
     * @return True if the GUI must be rendered over the Help system
     */
    bool MustRenderOverHelpSystem() const override { return true; }

  public:
    /*!
     * @brief Constructore
     * @param window Target window
     * @param title Title
     * @param initValue Initial value
     * @param okCallback Callback interface
     */
    GuiArcadeVirtualKeyboard(WindowManager& window, const std::string& title, const std::string& initValue,
                             IGuiArcadeVirtualKeyboardInterface* okCallback);

    /*!
     * @brief Destructor
     */
    ~GuiArcadeVirtualKeyboard() override
    {
      SDL_StopTextInput();
    }

    /*!
     * Get the current text text
     */
    std::string Text() const { return Strings::UnicodeToUtf8(mText); }
};

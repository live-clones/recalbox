#pragma once

#include "platform.h"
#include "platform_gl.h"

#include "GuiComponent.h"
#include <string>
#include <memory>
#include <utils/datetime/HighResolutionTimer.h>
#include <RecalboxConf.h>
#include "resources/TextureResource.h"

class VideoComponent : public GuiComponent
{
  private:
    enum State
    {
      Uninitialized,
      InitializeVideo,
      StartVideo,
      DisplayVideo,
      StopVideo,
    };

    enum Effect
    {
      Bump,
      Fade,
      BreakingNews,
      _LastItem,
    };

    //! Video state
    State mState;

    //! Video effect
    Effect mEffect;

    Vector2f mTargetSize;

    std::string mVideoPath;
    bool mTargetIsMax;

    // Calculates the correct mSize from our resizing information (set by setResize/setMaxSize).
    // Used internally whenever the resizing parameters or texture change.
    void resize();

    struct Vertex
    {
      Vector2f pos;
      Vector2f tex;
    } mVertices[6];

    GLubyte mColors[6*4];

    void updateVertices(double bump);
    void updateColors();

    unsigned int mColorShift;

    unsigned char mFadeOpacity;
    bool mFading;
    bool mForceLoad;
    bool mDynamic;

    //! Time to vidéo
    int mVideoDelay;
    //! Effect timeing
    int mVideoEffect;
    //! Video loop (0 for infinite loop)
    int mVideoLoop;

    //! High reolution timer for time computations
    HighResolutionTimer mTimer;

    /*!
     * @brief Decide what to display
     * @param effect [out] Set to the video bump value
     * @return bool if the video frame has to be displayed
     */
    bool ProcessDisplay(double& effect);

    /*!
     * @brief Stop video and reset component context
     */
    void ResetAnimations();

    /*!
     * @brief Process effect and return a double from 0.0 to 1.0
     * @param elapsedms elapsed time in millisecond
     * @param in True if the effect is used in starting video phase. False in stopping video phase
     * @return 0.0 to 1.0 value.
     */
    double ProcessEffect(int elapsedms, bool in);

    //! Pi constant
    static constexpr double Pi = 3.1415926535;

  public:
    explicit VideoComponent(Window* window, bool forceLoad = false, bool dynamic = true);
    ~VideoComponent() override = default;

    /*!
     * @brief Set video
     * @param path Path to video file
     * @param delay Delay before actually playing the video in milliseconds
     * @param loops Number of loops (0 = infinite loops)
     */
    void setVideo(const std::string& path, int delay, int loops);

    void setOpacity(unsigned char opacity) override;

    // Resize the image to fit this size. If one axis is zero, scale that axis to maintain aspect ratio.
    // If both are non-zero, potentially break the aspect ratio.  If both are zero, no resizing.
    // Can be set before or after an image is loaded.
    // setMaxSize() and setResize() are mutually exclusive.
    void setResize(float width, float height);
    inline void setResize(const Vector2f& size) { setResize(size.x(), size.y()); }

    // Resize the image to be as large as possible but fit within a box of this size.
    // Can be set before or after an image is loaded.
    // Never breaks the aspect ratio. setMaxSize() and setResize() are mutually exclusive.
    void setMaxSize(float width, float height);
    inline void setMaxSize(const Vector2f& size) { setMaxSize(size.x(), size.y()); }

    // Multiply all pixels in the image by this color when rendering.
    void setColorShift(unsigned int color);
    void setColor(unsigned int color) override;

    void render(const Transform4x4f& parentTrans) override;

    void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) override;

    std::vector<HelpPrompt> getHelpPrompts() override;

    static constexpr int DEFAULT_VIDEODELAY = 2000;
    static constexpr int DEFAULT_VIDEOEFFET = 500;
    static constexpr int DEFAULT_VIDEOLOOP  = 1;
};

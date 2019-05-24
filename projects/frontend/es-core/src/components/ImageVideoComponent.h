//
// Created by bkg2k on 16/05/19.
//

#pragma once

#include "platform.h"
#include "platform_gl.h"

#include "GuiComponent.h"
#include <string>
#include <memory>
#include <util/HighResolutionTimer.h>
#include "resources/TextureResource.h"

class ImageVideoComponent : public GuiComponent
{
  private:
    enum State
    {
      DisplayImage,
      InitializeVideo,
      BumpVideo,
      DisplayVideo,
      FinalizeVideo,
    };

    State mState;

    Eigen::Vector2f mTargetSize;

    std::string mPath;
    std::string mVideoPath;
    bool mTargetIsMax;

    // Calculates the correct mSize from our resizing information (set by setResize/setMaxSize).
    // Used internally whenever the resizing parameters or texture change.
    void resize();

    typedef struct Vertex
    {
      Eigen::Vector2f pos;
      Eigen::Vector2f tex;
    } RectangleVertexes[6];

    RectangleVertexes mVertices;

    GLubyte mColors[6*4];

    void updateVertices();
    void updateColors();

    unsigned int mColorShift;

    std::shared_ptr<TextureResource> mTexture;
    unsigned char mFadeOpacity;
    bool mFading;
    bool mDynamic;

    HighResolutionTimer mTimer;

    static constexpr int IMAGE_TIMING = 5000;
    static constexpr int IMAGE_TO_VIDEO_TIMING = 500;

    /*!
     * @brief Decide what to display
     * @param video [out] Set if we must display the video
     * @param image [out] Set if we must display the image
     * @param bumpEffect [out] Set to the video bump value
     */
    void ProcessDisplay(bool& video, bool&image, double& bumpEffect);

    /*!
     * @brief Calculate new bumped vertexes
     * @param bump
     */
    void CalculateVideoVextexes(RectangleVertexes& destinationVertexes, double bump);

  public:
    explicit ImageVideoComponent(Window* window, bool dynamic = true);
    ~ImageVideoComponent() override;

    // Loads the image at the given filepath. Will tile if tile is true (retrieves texture as tiling, creates vertices accordingly).
    void setImage(const std::string& path, bool tile = false);
    // Loads an image from memory.
    void setImage(const char* image, size_t length, bool tile = false);
    // Use an already existing texture.
    void setImage(const std::shared_ptr<TextureResource>& texture);

    // Set video file path
    void setVideo(const std::string& path);

    void onSizeChanged() override;
    void setOpacity(unsigned char opacity) override;

    Eigen::Vector2f getTargetSize() const { return mTargetSize; }

    // Resize the image to fit this size. If one axis is zero, scale that axis to maintain aspect ratio.
    // If both are non-zero, potentially break the aspect ratio.  If both are zero, no resizing.
    // Can be set before or after an image is loaded.
    // setMaxSize() and setResize() are mutually exclusive.
    void setResize(float width, float height);
    inline void setResize(const Eigen::Vector2f& size) { setResize(size.x(), size.y()); }

    // Resize the image to be as large as possible but fit within a box of this size.
    // Can be set before or after an image is loaded.
    // Never breaks the aspect ratio. setMaxSize() and setResize() are mutually exclusive.
    void setMaxSize(float width, float height);
    void setNormalisedMaxSize(float width, float height);
    inline void setMaxSize(const Eigen::Vector2f& size) { setMaxSize(size.x(), size.y()); }

    // Multiply all pixels in the image by this color when rendering.
    void setColorShift(unsigned int color);
    void setColor(unsigned int color) override;

    // Returns the size of the current texture, or (0, 0) if none is loaded.  May be different than drawn size (use getSize() for that).
    Eigen::Vector2i getTextureSize() const;

    bool hasImage();

    void render(const Eigen::Affine3f& parentTrans) override;

    void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) override;

    std::vector<HelpPrompt> getHelpPrompts() override;

    void ResetAnimations();
};

//
// Created by bkg2k on 16/05/19.
//

#include "components/ImageVideoComponent.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <math.h>
#include <datetime/DateTime.h>
#include "Log.h"
#include "Renderer.h"
#include "ThemeData.h"
#include "Util.h"
#include "Locale.h"

#include "VideoEngine.h"

Eigen::Vector2i ImageVideoComponent::getTextureSize() const {
  if (mTexture) {
    return mTexture->getSize();
  }
  return Eigen::Vector2i::Zero();
}

ImageVideoComponent::ImageVideoComponent(Window* window, bool dynamic)
  : GuiComponent(window),
    mState(State::DisplayImage),
    mTargetSize(0, 0),
    mPath(""),
    mTargetIsMax(false),
    mColorShift(0xFFFFFFFF),
    mFadeOpacity(0.0f),
    mFading(false),
    mDynamic(dynamic)
{
  updateColors();
}

ImageVideoComponent::~ImageVideoComponent() {
}

void ImageVideoComponent::resize() {
  if (!mTexture) {
    return;
  }

  const Eigen::Vector2f textureSize = mTexture->getSourceImageSize();
  if (textureSize.isZero())
    return;

  if (mTexture->isTiled()) {
    mSize = mTargetSize;
  } else {
    // SVG rasterization is determined by height (see SVGResource.cpp), and rasterization is done in terms of pixels
    // if rounding is off enough in the rasterization step (for images with extreme aspect ratios), it can cause cutoff when the aspect ratio breaks
    // so, we always make sure the resultant height is an integer to make sure cutoff doesn't happen, and scale width from that 
    // (you'll see this scattered throughout the function)
    // this is probably not the best way, so if you're familiar with this problem and have a better solution, please make a pull request!

    if (mTargetIsMax) {
      mSize = textureSize;

      Eigen::Vector2f resizeScale((mTargetSize.x() / mSize.x()), (mTargetSize.y() / mSize.y()));

      if (resizeScale.x() < resizeScale.y()) {
        mSize[0] *= resizeScale.x();
        mSize[1] *= resizeScale.x();
      } else {
        mSize[0] *= resizeScale.y();
        mSize[1] *= resizeScale.y();
      }

      // for SVG rasterization, always calculate width from rounded height (see comment above)
      mSize[1] = round(mSize[1]);
      mSize[0] = (mSize[1] / textureSize.y()) * textureSize.x();

    } else {
      // if both components are set, we just stretch
      // if no components are set, we don't resize at all
      mSize = mTargetSize.isZero() ? textureSize : mTargetSize;

      // if only one component is set, we resize in a way that maintains aspect ratio
      // for SVG rasterization, we always calculate width from rounded height (see comment above)
      if (!mTargetSize.x() && mTargetSize.y()) {
        mSize[1] = round(mTargetSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      } else if (mTargetSize.x() && !mTargetSize.y()) {
        mSize[1] = round((mTargetSize.x() / textureSize.x()) * textureSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      }
    }
  }

  // mSize.y() should already be rounded
  mTexture->rasterizeAt((int)round(mSize.x()), (int)round(mSize.y()));

  onSizeChanged();
}

void ImageVideoComponent::onSizeChanged() {
  updateVertices();
}

void ImageVideoComponent::setVideo(const std::string& path)
{
  mVideoPath = path;
}

void ImageVideoComponent::setImage(const std::string& path, bool tile) {
  if ( (mPath == path) && mTexture && (mTexture->isTiled() == tile) ) {
    return;
  }
  mPath = path;

  if (path.empty() || !ResourceManager::getInstance()->fileExists(path)) {
    mTexture.reset();
  } else {
    mTexture = TextureResource::get(path, tile, true, mDynamic);
  }
  resize();
}

/**
 * Set image from data scrapped
 */
void ImageVideoComponent::setImage(const char* image, size_t length, bool tile) {
  mPath = "!";
  mTexture.reset();
  mTexture = TextureResource::get("", tile);
  mTexture->initFromMemory(image, length);
  resize();
}


void ImageVideoComponent::setImage(const std::shared_ptr<TextureResource>& texture) {
  mPath = "!";
  mTexture = texture;
  resize();
}

void ImageVideoComponent::setResize(float width, float height) {
  mTargetSize << width, height;
  mTargetIsMax = false;
  resize();
}

void ImageVideoComponent::setMaxSize(float width, float height) {
  mTargetSize << width, height;
  mTargetIsMax = true;
  resize();
}

void ImageVideoComponent::setNormalisedMaxSize(float width, float height) {
  Eigen::Vector2f pos = denormalise(width, height);
  setMaxSize(pos.x(), pos.y());
}

void ImageVideoComponent::setColorShift(unsigned int color) {
  mColorShift = color;
  // Grab the opacity from the color shift because we may need to apply it if
  // fading textures in
  mOpacity = color & 0xffU;
  updateColors();
}

void ImageVideoComponent::setColor(unsigned int color) {
  setColorShift(color);
}

void ImageVideoComponent::setOpacity(unsigned char opacity) {
  mOpacity = opacity;
  mColorShift = ((mColorShift >> 8U) << 8U) | mOpacity;
  updateColors();
}

void ImageVideoComponent::updateVertices() {
  if (!mTexture || !mTexture->isInitialized()) {
    return;
  }

  // we go through this mess to make sure everything is properly rounded
  // if we just round vertices at the end, edge cases occur near sizes of 0.5
  Eigen::Vector2f topLeft(0.0, 0.0);
  Eigen::Vector2f bottomRight(round(mSize.x()), round(mSize.y()));

  mVertices[0].pos << topLeft.x(), topLeft.y();
  mVertices[1].pos << topLeft.x(), bottomRight.y();
  mVertices[2].pos << bottomRight.x(), topLeft.y();

  mVertices[3].pos << bottomRight.x(), topLeft.y();
  mVertices[4].pos << topLeft.x(), bottomRight.y();
  mVertices[5].pos << bottomRight.x(), bottomRight.y();

  float px = 1.0f, py = 1.0f;
  if (mTexture->isTiled())
  {
    px = mSize.x() / getTextureSize().x();
    py = mSize.y() / getTextureSize().y();
  }

  mVertices[0].tex << 0, py;
  mVertices[1].tex << 0, 0;
  mVertices[2].tex << px, py;

  mVertices[3].tex << px, py;
  mVertices[4].tex << 0, 0;
  mVertices[5].tex << px, 0;
}

void ImageVideoComponent::CalculateVideoVextexes(RectangleVertexes& destinationVertexes, double bump)
{
  double bumpedWidth = mSize.x() * bump;
  double bumpedHeight = mSize.y() * bump;
  double centerX = mSize.x() / 2.0;
  double centerY = mSize.y() / 2.0;

  Eigen::Vector2f topLeft(round(centerX - bumpedWidth / 2.0), round(centerY - bumpedHeight / 2.0));
  Eigen::Vector2f bottomRight(round(centerX + bumpedWidth / 2.0), round(centerY + bumpedHeight / 2.0));

  destinationVertexes[0].pos << topLeft.x(), topLeft.y();
  destinationVertexes[1].pos << topLeft.x(), bottomRight.y();
  destinationVertexes[2].pos << bottomRight.x(), topLeft.y();

  destinationVertexes[3].pos << bottomRight.x(), topLeft.y();
  destinationVertexes[4].pos << topLeft.x(), bottomRight.y();
  destinationVertexes[5].pos << bottomRight.x(), bottomRight.y();

  destinationVertexes[0].tex << 0.0f, 0.0f;
  destinationVertexes[1].tex << 0.0f, 1.0f;
  destinationVertexes[2].tex << 1.0f, 0.0f;

  destinationVertexes[3].tex << 1.0f, 0.0f;
  destinationVertexes[4].tex << 0.0f, 1.0f;
  destinationVertexes[5].tex << 1.0f, 1.0f;
}

void ImageVideoComponent::updateColors() {
  Renderer::buildGLColorArray(mColors, mColorShift, 6);
}

void ImageVideoComponent::ResetAnimations()
{
  LOG(LogDebug) << "Animations reseted!";

  mTimer.Initialize(0);
  mState = State::DisplayImage;
  LOG(LogDebug) << "Timer reseted: State::DisplayImage " + DateTime().ToPreciseTimeStamp();

  // Stop the video
  if (VideoEngine::This().IsPlaying())
    VideoEngine::This().StopVideo();
}

void ImageVideoComponent::ProcessDisplay(bool& video, bool&image, double& bumpEffect)
{
  int elapsed = mTimer.GetMilliSeconds();

  switch(mState)
  {
    case State::DisplayImage:
    {
      video = false;
      image = true;
      bumpEffect = 0.0;
      if (elapsed >= IMAGE_TIMING && !mVideoPath.empty())
      {
        mState = State::InitializeVideo;
        mTimer.Initialize(0);
        LOG(LogDebug) << "Timer reseted: State::InitializeVideo " << DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::InitializeVideo:
    {
      // Start video if it's not started yet
      if (VideoEngine::This().IsIdle())
        VideoEngine::This().PlayVideo(mVideoPath);
      video = false;
      image = true;
      bumpEffect = 0.0;
      if (VideoEngine::This().IsPlaying())
      {
        mState = State::BumpVideo;
        mTimer.Initialize(0);
        LOG(LogDebug) << "Timer reseted: State::BumpVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::BumpVideo:
    {
      video = image = true;
      static constexpr double Pi = 3.1415926535;
      double position = (double)elapsed / IMAGE_TO_VIDEO_TIMING;
      double sinus = sin((Pi / 2.0) * position) + sin(Pi * position) / 2.0;
      bumpEffect = sinus;
      LOG(LogDebug) << "Bump: " << bumpEffect;
      if (elapsed >= IMAGE_TO_VIDEO_TIMING)
      {
        mState = State::DisplayVideo;
        mTimer.Initialize(0);
        LOG(LogDebug) << "Timer reseted: State::DisplayVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::DisplayVideo:
    {
      // Video only
      video = true;
      image = false;
      bumpEffect = 1.0;
      if (elapsed >= VideoEngine::This().GetVideoDurationMs() - IMAGE_TO_VIDEO_TIMING)
      {
        mState = State::FinalizeVideo;
        mTimer.Initialize(0);
        LOG(LogDebug) << "Timer reseted: State::FinalizeVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::FinalizeVideo:
    {
      video = image = true;
      static constexpr double Pi = 3.1415926535;
      double position = (double)(IMAGE_TO_VIDEO_TIMING - elapsed) / IMAGE_TO_VIDEO_TIMING;
      double sinus = sin((Pi / 2.0) * position) + sin(Pi * position) / 2.0;
      bumpEffect = sinus;
      if (elapsed >= IMAGE_TO_VIDEO_TIMING)
      {
        mState = State::DisplayImage;
        VideoEngine::This().StopVideo();
        mTimer.Initialize(0);
        LOG(LogDebug) << "Timer reseted: State::DisplayImage " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
  }
}


void ImageVideoComponent::render(const Eigen::Affine3f& parentTrans)
{
  Eigen::Affine3f trans = parentTrans * getTransform();
  Renderer::setMatrix(trans);

  bool displayVideo = false;
  bool displayImage = false;
  double bumpEffect = 0.0;
  ProcessDisplay(displayVideo, displayImage, bumpEffect);

  // Render image
  if (displayImage)
    if (mTexture && mOpacity > 0)
    {
      if (mTexture->isInitialized())
      {
        // actually draw the image
        mTexture->bind();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &mVertices[0].pos);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &mVertices[0].tex);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, mColors);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
      }
      else
      {
        LOG(LogError) << "Image texture is not initialized!";
        mTexture.reset();
      }
    }

  if (displayVideo)
  {
    TextureData& videoFrame = VideoEngine::This().GetDisplayableFrame();
    videoFrame.uploadAndBind();

    RectangleVertexes videoVertices;
    CalculateVideoVextexes(videoVertices, bumpEffect);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &videoVertices[0].pos);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &videoVertices[0].tex);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, mColors);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  }

  GuiComponent::renderChildren(trans);
}

bool ImageVideoComponent::hasImage() {
  return (bool)mTexture;
}

void ImageVideoComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) {
  using namespace ThemeFlags;

  const ThemeData::ThemeElement* elem = theme->getElement(view, element, "image");
  if (!elem) {
    return;
  }

  Eigen::Vector2f scale = getParent() ? getParent()->getSize() : Eigen::Vector2f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

  if (properties & POSITION && elem->has("pos")) {
    Eigen::Vector2f denormalized = elem->get<Eigen::Vector2f>("pos").cwiseProduct(scale);
    setPosition(Eigen::Vector3f(denormalized.x(), denormalized.y(), 0));
  }

  if (properties & ThemeFlags::SIZE) {
    if (elem->has("size")) {
      setResize(elem->get<Eigen::Vector2f>("size").cwiseProduct(scale));
    } else if (elem->has("maxSize")) {
      setMaxSize(elem->get<Eigen::Vector2f>("maxSize").cwiseProduct(scale));
    }
  }

  // position + size also implies origin
  if ((properties & ORIGIN || (properties & POSITION && properties & ThemeFlags::SIZE)) && elem->has("origin")) {
    setOrigin(elem->get<Eigen::Vector2f>("origin"));
  }

  if (properties & PATH && elem->has("path")) {
    bool tile = (elem->has("tile") && elem->get<bool>("tile"));
    setImage(elem->get<std::string>("path"), tile);
  }

  if (properties & COLOR && elem->has("color")) {
    setColorShift(elem->get<unsigned int>("color"));
  }

  if (properties & ThemeFlags::ROTATION) {
    if (elem->has("rotation")) {
      setRotationDegrees(elem->get<float>("rotation"));
    }
    if (elem->has("rotationOrigin")) {
      setRotationOrigin(elem->get<Eigen::Vector2f>("rotationOrigin"));
    }
  }

  if (properties & ThemeFlags::Z_INDEX && elem->has("zIndex")) {
    setZIndex(elem->get<float>("zIndex"));
  } else {
    setZIndex(getDefaultZIndex());
  }
}

std::vector<HelpPrompt> ImageVideoComponent::getHelpPrompts() {
  std::vector<HelpPrompt> ret;
  ret.push_back(HelpPrompt("b", _("SELECT")));
  return ret;
}

#include "components/VideoComponent.h"
#include <iostream>
#include <math.h>
#include <VideoEngine.h>
#include <utils/datetime/DateTime.h>
#include "Log.h"
#include "Renderer.h"
#include "ThemeData.h"
#include "Locale.h"

VideoComponent::VideoComponent(Window* window, bool forceLoad, bool dynamic)
: GuiComponent(window),
  mState(State::Uninitialized),
  mEffect(Effect::BreakingNews),
  mVideoPath(""),
  mTargetIsMax(false),
  mColors(),
  mColorShift(0xFFFFFFFF),
  mFadeOpacity(0),
  mFading(false),
  mForceLoad(forceLoad),
  mDynamic(dynamic),
  mVideoDelay(DEFAULT_VIDEODELAY),
  mVideoEffect(DEFAULT_VIDEOEFFET),
  mVideoLoop(DEFAULT_VIDEOLOOP)
{
  updateColors();
}

void VideoComponent::resize()
{
  TextureData& texture = VideoEngine::This().GetDisplayableFrame();

  const Vector2f textureSize(texture.width(), texture.height());
  if (textureSize.isZero()) return;

  if (texture.tiled())
  {
    mSize = mTargetSize;
  }
  else
  {
    // SVG rasterization is determined by height (see SVGResource.cpp), and rasterization is done in terms of pixels
    // if rounding is off enough in the rasterization step (for images with extreme aspect ratios), it can cause cutoff when the aspect ratio breaks
    // so, we always make sure the resultant height is an integer to make sure cutoff doesn't happen, and scale width from that
    // (you'll see this scattered throughout the function)
    // this is probably not the best way, so if you're familiar with this problem and have a better solution, please make a pull request!

    if (mTargetIsMax)
    {
      mSize = textureSize;

      Vector2f resizeScale((mTargetSize.x() / mSize.x()), (mTargetSize.y() / mSize.y()));

      if (resizeScale.x() < resizeScale.y())
      {
        mSize[0] *= resizeScale.x();
        mSize[1] *= resizeScale.x();
      }
      else
      {
        mSize[0] *= resizeScale.y();
        mSize[1] *= resizeScale.y();
      }

      // for SVG rasterization, always calculate width from rounded height (see comment above)
      mSize[1] = round(mSize[1]);
      mSize[0] = (mSize[1] / textureSize.y()) * textureSize.x();
    }
    else
    {
      // if both components are set, we just stretch
      // if no components are set, we don't resize at all
      mSize = mTargetSize.isZero() ? textureSize : mTargetSize;

      // if only one component is set, we resize in a way that maintains aspect ratio
      // for SVG rasterization, we always calculate width from rounded height (see comment above)
      if ((mTargetSize.x() == 0) && (mTargetSize.y() != 0))
      {
        mSize[1] = round(mTargetSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      }
      else if ((mTargetSize.x() != 0) && (mTargetSize.y() == 0))
      {
        mSize[1] = round((mTargetSize.x() / textureSize.x()) * textureSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      }
    }
  }
}

void VideoComponent::setVideo(const std::string& path, int delay, int loops)
{
  VideoEngine::This().StopVideo(false);
  mVideoPath = path;
  mVideoDelay = delay;
  mVideoLoop = loops;
  ResetAnimations();
}

void VideoComponent::setResize(float width, float height)
{
  mTargetSize.Set(width, height);
  mTargetIsMax = false;
  resize();
}

void VideoComponent::setMaxSize(float width, float height)
{
  mTargetSize.Set(width, height);
  mTargetIsMax = true;
  resize();
}

void VideoComponent::setColorShift(unsigned int color)
{
  mColorShift = color;
  // Grab the opacity from the color shift because we may need to apply it if
  // fading textures in
  mOpacity = color & 0xffU;
  updateColors();
}

void VideoComponent::setColor(unsigned int color)
{
  setColorShift(color);
}

void VideoComponent::setOpacity(unsigned char opacity)
{
  mOpacity = opacity;
  mColorShift = ((mColorShift >> 8U) << 8U) | mOpacity;
  updateColors();
}

void VideoComponent::updateVertices(double bump)
{
  switch(mEffect)
  {
    case Effect::Bump:
    case Effect::BreakingNews:
    {
      double bumpedWidth = mSize.x() * bump;
      double bumpedHeight = mSize.y() * bump;
      double centerX = mSize.x() / 2.0;
      double centerY = mSize.y() / 2.0;

      Vector2f topLeft(round(centerX - bumpedWidth / 2.0), round(centerY - bumpedHeight / 2.0));
      Vector2f bottomRight(round(centerX + bumpedWidth / 2.0), round(centerY + bumpedHeight / 2.0));

      mVertices[0].pos.Set(topLeft.x(), topLeft.y());
      mVertices[1].pos.Set(topLeft.x(), bottomRight.y());
      mVertices[2].pos.Set(bottomRight.x(), topLeft.y());

      mVertices[3].pos.Set(bottomRight.x(), topLeft.y());
      mVertices[4].pos.Set(topLeft.x(), bottomRight.y());
      mVertices[5].pos.Set(bottomRight.x(), bottomRight.y());
      break;
    }
    case Effect::Fade:
    {
      Vector2f topLeft(0.0, 0.0);
      Vector2f bottomRight(round(mSize.x()), round(mSize.y()));

      mVertices[0].pos.Set(topLeft.x(), topLeft.y());
      mVertices[1].pos.Set(topLeft.x(), bottomRight.y());
      mVertices[2].pos.Set(bottomRight.x(), topLeft.y());

      mVertices[3].pos.Set(bottomRight.x(), topLeft.y());
      mVertices[4].pos.Set(topLeft.x(), bottomRight.y());
      mVertices[5].pos.Set(bottomRight.x(), bottomRight.y());
    }
    case Effect::_LastItem: break;
  }

  mVertices[0].tex.Set(0.0f, 0.0f);
  mVertices[1].tex.Set(0.0f, 1.0f);
  mVertices[2].tex.Set(1.0f, 0.0f);

  mVertices[3].tex.Set(1.0f, 0.0f);
  mVertices[4].tex.Set(0.0f, 1.0f);
  mVertices[5].tex.Set(1.0f, 1.0f);
}

void VideoComponent::updateColors()
{
  Renderer::buildGLColorArray(mColors, mColorShift, 6);
}

void VideoComponent::ResetAnimations()
{
  LOG(LogDebug) << "Animations reseted!";

  mTimer.Initialize(0);
  mState = State::Uninitialized;
  // LOG(LogDebug) << "Timer reseted: State::Uninitialized " + DateTime().ToPreciseTimeStamp();

  // Stop the video
  if (VideoEngine::This().IsPlaying())
    VideoEngine::This().StopVideo();
}

double VideoComponent::ProcessEffect(int elapsedms, bool in)
{
  double linear = (double) (in ? elapsedms : (mVideoEffect - elapsedms)) / mVideoEffect;

  switch(mEffect)
  {
    case Effect::Bump:
    {
      double sinus = sin((Pi / 2.0) * linear) + sin(Pi * linear) / 2.0;
      return sinus;
    }
    case Effect::Fade:
    case Effect::BreakingNews:
    {
      if (linear < 0.0) linear = 0.0;
      if (linear > 1.0) linear = 1.0;
      return linear;
    }
    case Effect::_LastItem: break;
  }

  return 1.0;
}

bool VideoComponent::ProcessDisplay(double& effect)
{
  int elapsed = mTimer.GetMilliSeconds();
  bool video = false;

  switch (mState)
  {
    case State::Uninitialized:
    {
      effect = 0.0;
      if (elapsed >= mVideoDelay && !mVideoPath.empty())
      {
        mEffect = (Effect)(((int)mEffect + 1) % (int)Effect::_LastItem);
        mState = State::InitializeVideo;
        mTimer.Initialize(0);
        // LOG(LogDebug) << "Timer reseted: State::InitializeVideo " << DateTime().ToPreciseTimeStamp() << " elapsed: "  << elapsed;
      }
      break;
    }
    case State::InitializeVideo:
    {
      // Start video if it's not started yet
      if (VideoEngine::This().IsIdle())
        VideoEngine::This().PlayVideo(mVideoPath);
      effect = 0.0;
      if (VideoEngine::This().IsPlaying())
      {
        resize();
        mState = State::StartVideo;
        mTimer.Initialize(0);
        // LOG(LogDebug) << "Timer reseted: State::BumpVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::StartVideo:
    {
      video = true;
      effect = ProcessEffect(elapsed, true);
      if (elapsed >= mVideoEffect)
      {
        mState = State::DisplayVideo;
        mTimer.Initialize(0);
        // LOG(LogDebug) << "Timer reseted: State::DisplayVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::DisplayVideo:
    {
      // Video only
      video = true;
      effect = 1.0;
      if ((mVideoLoop > 0) && (elapsed >= VideoEngine::This().GetVideoDurationMs() * mVideoLoop - mVideoEffect))
      {
        mState = State::StopVideo;
        mTimer.Initialize(0);
        //LOG(LogDebug) << "Timer reseted: State::FinalizeVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
    case State::StopVideo:
    {
      video = true;
      effect = ProcessEffect(elapsed, false);
      if (elapsed >= mVideoEffect)
      {
        mState = State::Uninitialized;
        VideoEngine::This().StopVideo();
        mTimer.Initialize(0);
        //LOG(LogDebug) << "Timer reseted: State::DisplayImage " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed;
      }
      break;
    }
  }

  return video;
}

void VideoComponent::render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();
  Renderer::setMatrix(trans);

  double effect = 0.0;
  bool display = ProcessDisplay(effect);

  if (display)
  {
    TextureData& videoFrame = VideoEngine::This().GetDisplayableFrame();
    videoFrame.uploadAndBind();
    // Bounds
    updateVertices(effect);
    // Opacity
    setOpacity((mEffect == Effect::Fade) ? (int)(effect * 255.0) : 255);
    updateColors();
    // Rotation
    setRotation(mEffect == Effect::BreakingNews ? ((float)(Pi * 4)) * (float)effect : 0.0);
    setRotationOrigin(0.5f, 0.5f);

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

  GuiComponent::renderChildren(trans);
}

/*void VideoComponent::fadeIn(bool textureLoaded)
{
  if (!mForceLoad)
  {
    if (!textureLoaded)
    {
      // Start the fade if this is the first time we've encountered the unloaded texture
      if (!mFading)
      {
        // Start with a zero opacity and flag it as fading
        mFadeOpacity = 0;
        mFading = true;
        // Set the colours to be translucent
        mColorShift = ((mColorShift >> 8U) << 8U) | 0U;
        updateColors();
      }
    }
    else if (mFading)
    {
      // The texture is loaded and we need to fade it in. The fade is based on the frame rate
      // and is 1/4 second if running at 60 frames per second although the actual value is not
      // that important
      int opacity = mFadeOpacity + 255 / 15;
      // See if we've finished fading
      if (opacity >= 255)
      {
        mFadeOpacity = 255;
        mFading = false;
      }
      else
      {
        mFadeOpacity = (unsigned char) opacity;
      }
      // Apply the combination of the target opacity and current fade
      float newOpacity = (float) mOpacity * ((float) mFadeOpacity / 255.0f);
      mColorShift = ((mColorShift >> 8U) << 8U) | (unsigned char) newOpacity;
      updateColors();
    }
  }
}*/

void VideoComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element,
                           unsigned int properties)
{
  using namespace ThemeFlags;

  const ThemeData::ThemeElement* elem = theme->getElement(view, element, "video");
  if (!elem)
  {
    return;
  }

  Vector2f scale = getParent() ? getParent()->getSize() : Vector2f((float) Renderer::getScreenWidth(),
                                                                                 (float) Renderer::getScreenHeight());

  if (properties & POSITION && elem->has("pos"))
  {
    Vector2f denormalized = elem->get<Vector2f>("pos") * scale;
    setPosition(Vector3f(denormalized.x(), denormalized.y(), 0));
  }

  if (properties & ThemeFlags::SIZE)
  {
    if (elem->has("size"))
    {
      setResize(elem->get<Vector2f>("size") * scale);
    }
    else if (elem->has("maxSize"))
    {
      setMaxSize(elem->get<Vector2f>("maxSize") * scale);
    }
  }

  // position + size also implies origin
  if ((properties & ORIGIN || (properties & POSITION && properties & ThemeFlags::SIZE)) && elem->has("origin"))
  {
    setOrigin(elem->get<Vector2f>("origin"));
  }

  if (properties & PATH && elem->has("path"))
  {
    setVideo(elem->get<std::string>("path"), DEFAULT_VIDEODELAY, DEFAULT_VIDEOLOOP);
  }

  if (properties & COLOR && elem->has("color"))
  {
    setColorShift(elem->get<unsigned int>("color"));
  }

  if (properties & ThemeFlags::ROTATION)
  {
    if (elem->has("rotation"))
    {
      setRotationDegrees(elem->get<float>("rotation"));
    }
    if (elem->has("rotationOrigin"))
    {
      setRotationOrigin(elem->get<Vector2f>("rotationOrigin"));
    }
  }

  if (properties & ThemeFlags::Z_INDEX && elem->has("zIndex"))
  {
    setZIndex(elem->get<float>("zIndex"));
  }
  else
  {
    setZIndex(getDefaultZIndex());
  }
}

std::vector<HelpPrompt> VideoComponent::getHelpPrompts()
{
  return { HelpPrompt("b", _("SELECT")) };
}

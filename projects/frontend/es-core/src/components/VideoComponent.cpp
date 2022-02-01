#include <components/VideoComponent.h>
#include <audio/AudioManager.h>
#include <VideoEngine.h>
#include <Renderer.h>
#include <help/Help.h>
#include <themes/ThemeData.h>
#include <utils/Log.h>
#include <utils/locale/LocaleHelper.h>

VideoComponent::VideoComponent(WindowManager&window)
: Component(window),
  mState(State::Uninitialized),
  mEffect(Effect::BreakingNews),
  mTargetSize(0),
  mVideoPath(""),
  mTargetIsMax(false),
  mVertices{ { { 0, 0 }, { 0, 0 } } },
  mColors{ 0 },
  mColorShift(0xFFFFFFFF),
  mFadeOpacity(0),
  mVideoDelay(DEFAULT_VIDEODELAY),
  mVideoEffect(DEFAULT_VIDEOEFFET),
  mVideoLoop(DEFAULT_VIDEOLOOP),
  mDecodeAudio(DEFAULT_VIDEODECODEAUDIO)
{
  updateColors();
}

void VideoComponent::resize()
{
  VideoEngine::Instance().AquireTexture();
  TextureData& texture = VideoEngine::Instance().GetDisplayableFrame();
  const Vector2f textureSize(texture.width(), texture.height());
  VideoEngine::Instance().ReleaseTexture();

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
      mSize[1] = Math::round(mSize[1]);
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
        mSize[1] = Math::round(mTargetSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      }
      else if ((mTargetSize.x() != 0) && (mTargetSize.y() == 0))
      {
        mSize[1] = Math::round((mTargetSize.x() / textureSize.x()) * textureSize.y());
        mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
      }
    }
  }
}

void VideoComponent::setVideo(const Path& path, int delay, int loops, bool decodeAudio)
{
  AudioManager::Instance().ResumeMusicIfNecessary();
  VideoEngine::Instance().StopVideo(true);
  mVideoPath = path;
  mVideoDelay = delay;
  mVideoLoop = loops;
  mDecodeAudio = decodeAudio;
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
      float bumpedWidth = (float)(mSize.x() * bump);
      float bumpedHeight = (float)(mSize.y() * bump);
      float centerX = mSize.x() / 2.0f;
      float centerY = mSize.y() / 2.0f;

      Vector2f topLeft(Math::round(centerX - bumpedWidth / 2.0f), Math::round(centerY - bumpedHeight / 2.0f));
      Vector2f bottomRight(Math::round(centerX + bumpedWidth / 2.0f), Math::round(centerY + bumpedHeight / 2.0f));

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
      Vector2f bottomRight(Math::round(mSize.x()), Math::round(mSize.y()));

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
  Renderer::BuildGLColorArray(mColors, mColorShift, 6);
}

void VideoComponent::ResetAnimations()
{
  { LOG(LogDebug) << "[VideoComponent] Animations reseted!"; }

  mTimer.Initialize(0);
  mState = State::Uninitialized;
  //{ LOG(LogDebug) << "[VideoComponent] Timer reseted: State::Uninitialized " + DateTime().ToPreciseTimeStamp(); }

  // Stop the video
  if (VideoEngine::Instance().IsPlaying())
    VideoEngine::Instance().StopVideo(false);
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
      if (elapsed >= mVideoDelay && !mVideoPath.IsEmpty())
      {
        mEffect = (Effect)(((int)mEffect + 1) % (int)Effect::_LastItem);
        mState = State::InitializeVideo;
        mTimer.Initialize(0);
        //{  LOG(LogDebug) << "[VideoComponent] Timer reseted: State::InitializeVideo " << DateTime().ToPreciseTimeStamp() << " elapsed: "  << elapsed; }
      }
      break;
    }
    case State::InitializeVideo:
    {
      // Start video if it's not started yet
      VideoEngine::Instance().PlayVideo(mVideoPath, mDecodeAudio);
      mState = State::WaitForVideoToStart;
      mTimer.Initialize(0);
      break;
    }
    case State::WaitForVideoToStart:
    {
      effect = 0.0;
      if (VideoEngine::Instance().IsPlaying())
      {
        resize();
        mState = State::StartVideo;
        mTimer.Initialize(0);
        //{ LOG(LogDebug) << "[VideoComponent] Timer reseted: State::BumpVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed; }
        AudioManager::Instance().PauseMusicIfNecessary();
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
        //{ LOG(LogDebug) << "[VideoComponent] Timer reseted: State::DisplayVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed; }
      }
      break;
    }
    case State::DisplayVideo:
    {
      // Video only
      video = true;
      effect = 1.0;
      if ((mVideoLoop > 0) && (elapsed >= VideoEngine::Instance().GetVideoDurationMs() * mVideoLoop - mVideoEffect))
      {
        mState = State::StopVideo;
        mTimer.Initialize(0);
        //{ LOG(LogDebug) << "[VideoComponent] Timer reseted: State::FinalizeVideo " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed; }
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
        VideoEngine::Instance().StopVideo(false);
        mTimer.Initialize(0);
        //{ LOG(LogDebug) << "[VideoComponent] Timer reseted: State::DisplayImage " + DateTime().ToPreciseTimeStamp() << " elapsed: " << elapsed; }
        video = false;
      }
      AudioManager::Instance().ResumeMusicIfNecessary();
      break;
    }
  }

  return video;
}

void VideoComponent::Render(const Transform4x4f& parentTrans)
{
    if(mDisabled)
    {
        return;
    }

  Transform4x4f trans = parentTrans * getTransform();
  Renderer::SetMatrix(trans);

  double effect = 0.0;
  bool display = ProcessDisplay(effect);

  if (display)
  {
    VideoEngine::Instance().AquireTexture();
    TextureData& videoFrame = VideoEngine::Instance().GetDisplayableFrame();
    videoFrame.uploadAndBind();
    VideoEngine::Instance().ReleaseTexture();

    // Bounds
    updateVertices(effect);
    // Opacity
    setOpacity((mEffect == Effect::Fade) ? (int)(effect * 255.0) : 255);
    updateColors();
    // Rotation
    setRotation(mEffect == Effect::BreakingNews ? (float)(Pi * 4.0) * (float)effect : 0.0f);
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

  Component::renderChildren(trans);
}

void VideoComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element,
                                ThemeProperties properties)
{
  const ThemeElement* elem = theme.getElement(view, element, "video");
  if (elem == nullptr)
  {
    return;
  }

  Vector2f scale = getParent() != nullptr ?
                   getParent()->getSize() :
                   Vector2f(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  if (hasFlag(properties, ThemeProperties::Position) && elem->HasProperty("pos"))
  {
    Vector2f denormalized = elem->AsVector("pos") * scale;
    setPosition(Vector3f(denormalized.x(), denormalized.y(), 0));
  }

  if (hasFlag(properties, ThemeProperties::Size))
  {
    if (elem->HasProperty("size"))
    {
      setResize(elem->AsVector("size") * scale);
    }
    else if (elem->HasProperty("maxSize"))
    {
      setMaxSize(elem->AsVector("maxSize") * scale);
    }
  }

  // position + size also implies origin
  if ((hasFlag(properties, ThemeProperties::Origin) || (hasFlags(properties, ThemeProperties::Position, ThemeProperties::Size))) && elem->HasProperty("origin"))
  {
    setOrigin(elem->AsVector("origin"));
  }

  if (hasFlag(properties, ThemeProperties::Path) && elem->HasProperty("path"))
  {
    setVideo(Path(elem->AsString("path")), DEFAULT_VIDEODELAY, DEFAULT_VIDEOLOOP, mDecodeAudio);
  }

  if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("color"))
  {
    setColorShift((unsigned int)elem->AsInt("color"));
  }

  if (hasFlag(properties, ThemeProperties::Rotation))
  {
    if (elem->HasProperty("rotation"))
    {
      setRotationDegrees(elem->AsFloat("rotation"));
    }
    if (elem->HasProperty("rotationOrigin"))
    {
      setRotationOrigin(elem->AsVector("rotationOrigin"));
    }
  }

  if (hasFlag(properties, ThemeProperties::ZIndex) && elem->HasProperty("zIndex"))
  {
    setZIndex(elem->AsFloat("zIndex"));
  }
  else
  {
    setZIndex(getDefaultZIndex());
  }
}

bool VideoComponent::getHelpPrompts(Help& help)
{
  help.Set(Help::Valid(), _("SELECT"));
  return true;
}

bool VideoComponent::isDiplayed() {
    return mState == State::DisplayVideo;
}
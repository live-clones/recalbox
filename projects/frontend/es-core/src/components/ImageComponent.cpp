#include <components/ImageComponent.h>
#include <utils/Log.h>
#include <Renderer.h>
#include <help/Help.h>
#include <themes/ThemeData.h>
#include <utils/locale/LocaleHelper.h>

Vector2i ImageComponent::getTextureSize() const {
    if (mTexture) {
        return mTexture->getSize();
    }
    return Vector2i::Zero();
}

ImageComponent::ImageComponent(WindowManager&window, bool forceLoad, bool dynamic)
  : Component(window)
  , mTargetSize(0, 0)
  , mPath()
  , mFlipX(false)
  , mFlipY(false)
  , mTargetIsMax(false)
  , mVertices{ { { 0, 0 }, { 0, 0 } } }
  , mColors{ 0 }
  , mColorShift(0xFFFFFFFF)
  , mOriginColor(0)
  , mFadeOpacity(0.0f)
  , mColorNotSet(true)
  , mFading(false)
  , mForceLoad(forceLoad)
  , mDynamic(dynamic)
  , mVisible(true)
{
  updateColors();
}

void ImageComponent::resize() {
    if (!mTexture) {
        return;
    }

    const Vector2f textureSize = mTexture->getSourceImageSize();
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

            Vector2f resizeScale((mTargetSize.x() / mSize.x()), (mTargetSize.y() / mSize.y()));
            
            if (resizeScale.x() < resizeScale.y()) {
                mSize[0] *= resizeScale.x();
                mSize[1] *= resizeScale.x();
            } else {
                mSize[0] *= resizeScale.y();
                mSize[1] *= resizeScale.y();
            }

            // for SVG rasterization, always calculate width from rounded height (see comment above)
            mSize[1] = Math::round(mSize[1]);
            mSize[0] = (mSize[1] / textureSize.y()) * textureSize.x();

        } else {
            // if both components are set, we just stretch
            // if no components are set, we don't resize at all
            mSize = mTargetSize.isZero() ? textureSize : mTargetSize;

            // if only one component is set, we resize in a way that maintains aspect ratio
            // for SVG rasterization, we always calculate width from rounded height (see comment above)
            if (mTargetSize.x() == 0 && mTargetSize.y() != 0) {
                mSize[1] = Math::round(mTargetSize.y());
                mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
            } else if (mTargetSize.x() != 0 && mTargetSize.y() == 0) {
                mSize[1] = Math::round((mTargetSize.x() / textureSize.x()) * textureSize.y());
                mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
            }
        }
    }

    // mSize.y() should already be rounded
    mTexture->rasterizeAt(Math::roundi(mSize.x()), Math::roundi(mSize.y()));

    onSizeChanged();
}

void ImageComponent::onSizeChanged() {
    updateVertices();
}

void ImageComponent::setImage(const Path& path, bool tile) {
    if ( (mPath == path) && mTexture && (mTexture->isTiled() == tile) ) {
        return;
    }
    mPath = path;

    if (path.IsEmpty() || !ResourceManager::fileExists(path)) {
        mTexture.reset();
    } else {
        mTexture = TextureResource::get(path, tile, mForceLoad, mDynamic);
    }
    resize();
}

/**
 * Set image from data scraped
 */
void ImageComponent::setImage(const char* image, size_t length, bool tile) {
    mPath = "!";
    mTexture.reset();
    mTexture = TextureResource::get(Path::Empty, tile);
    mTexture->initFromMemory(image, length);
    resize();
}


void ImageComponent::setImage(const std::shared_ptr<TextureResource>& texture) {
    mPath = "!";
    mTexture = texture;
    resize();
}

void ImageComponent::setResize(float width, float height) {
    mTargetSize.Set(width, height);
    mTargetIsMax = false;
    resize();
}

void ImageComponent::setMaxSize(float width, float height) {
    mTargetSize.Set(width, height);
    mTargetIsMax = true;
    resize();
}

void ImageComponent::setNormalisedMaxSize(float width, float height) {
    Vector2f pos = denormalise(width, height);
    setMaxSize(pos.x(), pos.y());
}

void ImageComponent::setFlipX(bool flip) {
    mFlipX = flip;
    updateVertices();
}

void ImageComponent::setFlipY(bool flip) {
    mFlipY = flip;
    updateVertices();
}

void ImageComponent::setColorShift(unsigned int color) {
    mColorShift = color;
    // Grab the opacity from the color shift because we may need to apply it if
    // fading textures in
    mOpacity = color & 0xff;

	if (mColorNotSet) {
		setOriginColor(mColorShift);
		mColorNotSet = false;
	}
    updateColors();
}

void ImageComponent::setColor(unsigned int color) {
    setColorShift(color);
}

void ImageComponent::setOpacity(unsigned char opacity) {
    mOpacity = opacity;
    mColorShift = (mColorShift >> 8 << 8) | mOpacity;
    updateColors();
}

void ImageComponent::updateVertices() {
    if (!mTexture || !mTexture->isInitialized()) {
        return;
    }

    // we go through this mess to make sure everything is properly rounded
    // if we just round vertices at the end, edge cases occur near sizes of 0.5
    Vector2f topLeft(0.0, 0.0);
    Vector2f bottomRight(Math::round(mSize.x()), Math::round(mSize.y()));

    mVertices[0].pos.Set(topLeft.x(), topLeft.y());
    mVertices[1].pos.Set(topLeft.x(), bottomRight.y());
    mVertices[2].pos.Set(bottomRight.x(), topLeft.y());

    mVertices[3].pos.Set(bottomRight.x(), topLeft.y());
    mVertices[4].pos.Set(topLeft.x(), bottomRight.y());
    mVertices[5].pos.Set(bottomRight.x(), bottomRight.y());

    float px = 1, py = 1;
    if (mTexture->isTiled()) {
        px = mSize.x() / (float)getTextureSize().x();
        py = mSize.y() / (float)getTextureSize().y();
    }

    mVertices[0].tex.Set(0, py);
    mVertices[1].tex.Set(0, 0);
    mVertices[2].tex.Set(px, py);

    mVertices[3].tex.Set(px, py);
    mVertices[4].tex.Set(0, 0);
    mVertices[5].tex.Set(px, 0);

    if (mFlipX)
    {
      for (auto & mVertice : mVertices)
            mVertice.tex[0] = mVertice.tex[0] == px ? 0 : px;
    }
    if (mFlipY) {
      for (auto & mVertice : mVertices)
            mVertice.tex[1] = mVertice.tex[1] == py ? 0 : py;
    }
}

void ImageComponent::updateColors() {
    Renderer::BuildGLColorArray(mColors, mColorShift, 6);
}

void ImageComponent::Render(const Transform4x4f& parentTrans)
{
    if(mThemeDisabled || !mVisible)
      return;

    Transform4x4f trans = parentTrans * getTransform();
    Renderer::SetMatrix(trans);
    
    if (mTexture && mOpacity > 0) {
        if (mTexture->isInitialized()) {
            // actually draw the image
            // The bind() function returns false if the texture is not currently loaded. A blank
            // texture is bound in this case but we want to handle a fade so it doesn't just 'jump' in
            // when it finally loads
            fadeIn(mTexture->bind());

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
        } else {
          { LOG(LogError) << "[ImageComponent] Image texture is not initialized!"; }
            mTexture.reset();
        }
    }

    Component::renderChildren(trans);
}

void ImageComponent::fadeIn(bool textureLoaded) {
    if (!mForceLoad) {
        if (!textureLoaded) {
            // Start the fade if this is the first time we've encountered the unloaded texture
            if (!mFading) {
                // Start with a zero opacity and flag it as fading
                mFadeOpacity = 0;
                mFading = true;
                // Set the colours to be translucent
                mColorShift = (mColorShift >> 8 << 8) | 0;
                updateColors();
            }
        }
        else if (mFading) {
            // The texture is loaded and we need to fade it in. The fade is based on the frame rate
            // and is 1/4 second if running at 60 frames per second although the actual value is not
            // that important
            int opacity = mFadeOpacity + 255 / 15;
            // See if we've finished fading
            if (opacity >= 255) {
                mFadeOpacity = 255;
                mFading = false;
            }
            else {
                mFadeOpacity = (unsigned char)opacity;
            }
            // Apply the combination of the target opacity and current fade
            float newOpacity = (float)mOpacity * ((float)mFadeOpacity / 255.0f);
            mColorShift = (mColorShift >> 8 << 8) | (unsigned char)newOpacity;
            updateColors();
        }
    }
}

void ImageComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
    const ThemeElement* elem = theme.getElement(view, element, "image");
    if (elem == nullptr) {
        return;
    }

    Vector2f scale = getParent() != nullptr ? getParent()->getSize() : Vector2f(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
    
    if (hasFlag(properties, ThemeProperties::Position) && elem->HasProperty("pos")) {
        Vector2f denormalized = elem->AsVector("pos") * scale;
        setPosition(Vector3f(denormalized.x(), denormalized.y(), 0));
    }

    if (hasFlag(properties,ThemeProperties::Size)) {
        if (elem->HasProperty("size")) {
            setResize(elem->AsVector("size") * scale);
        } else if (elem->HasProperty("maxSize")) {
            setMaxSize(elem->AsVector("maxSize") * scale);
        }
    }

    // position + size also implies origin
    if ((hasFlag(properties, ThemeProperties::Origin) || (hasFlags(properties, ThemeProperties::Position, ThemeProperties::Size))) && elem->HasProperty("origin")) {
        setOrigin(elem->AsVector("origin"));
    }

    if (hasFlag(properties, ThemeProperties::Path) && elem->HasProperty("path")) {
        bool tile = (elem->HasProperty("tile") && elem->AsBool("tile"));
        setImage(Path(elem->AsString("path")), tile);
    }

    if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("color")) {
        setColorShift((unsigned int)elem->AsInt("color"));
    }

    if (hasFlag(properties, ThemeProperties::Rotation)) {
        if (elem->HasProperty("rotation")) {
            setRotationDegrees(elem->AsFloat("rotation"));
        }
        if (elem->HasProperty("rotationOrigin")) {
            setRotationOrigin(elem->AsVector("rotationOrigin"));
        }
    }
    
    if (hasFlag(properties, ThemeProperties::ZIndex) && elem->HasProperty("zIndex")) {
        setZIndex(elem->AsFloat("zIndex"));
    } else {
        setZIndex(getDefaultZIndex());
    }

    if (hasFlag(properties, ThemeProperties::Position) && elem->HasProperty("disabled"))
    {
      mThemeDisabled = elem->AsBool("disabled");
    }
}

bool ImageComponent::getHelpPrompts(Help& help)
{
  help.Set(Help::Valid(), _("SELECT"));
  return true;
}

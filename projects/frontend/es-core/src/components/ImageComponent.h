#ifndef _IMAGECOMPONENT_H_
#define _IMAGECOMPONENT_H_

#include "platform.h"
#include "platform_gl.h"

#include "GuiComponent.h"
#include <string>
#include <memory>
#include "resources/TextureResource.h"

class ImageComponent : public GuiComponent
{
public:
	explicit ImageComponent(Window* window, bool forceLoad = false, bool dynamic = true);

	//Loads the image at the given filepath. Will tile if tile is true (retrieves texture as tiling, creates vertices accordingly).
    void setImage(const std::string& path, bool tile = false);
	//Loads an image from memory.
	void setImage(const char* image, size_t length, bool tile = false);
	//Use an already existing texture.
	void setImage(const std::shared_ptr<TextureResource>& texture);

	void onSizeChanged() override;
	void setOpacity(unsigned char opacity) override;

    Vector2f getTargetSize() const { return mTargetSize; }

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
	void setNormalisedMaxSize(float width, float height);
	inline void setMaxSize(const Vector2f& size) { setMaxSize(size.x(), size.y()); }

	// Multiply all pixels in the image by this color when rendering.
	void setColorShift(unsigned int color);
	void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color){mOriginColor = color;};
	inline unsigned int getOriginColor() override{return mOriginColor;};

	void setFlipX(bool flip); // Mirror on the X axis.
	void setFlipY(bool flip); // Mirror on the Y axis.

	// Returns the size of the current texture, or (0, 0) if none is loaded.  May be different than drawn size (use getSize() for that).
	Vector2i getTextureSize() const;

	bool hasImage();

	void render(const Transform4x4f& parentTrans) override;

	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) override;

	std::vector<HelpPrompt> getHelpPrompts() override;
private:
	Vector2f mTargetSize;

	std::string mPath;
	bool mFlipX, mFlipY, mTargetIsMax;

	// Calculates the correct mSize from our resizing information (set by setResize/setMaxSize).
	// Used internally whenever the resizing parameters or texture change.
	void resize();

	struct Vertex
	{
		Vector2f pos;
		Vector2f tex;
	} mVertices[6];

	GLubyte mColors[6*4];

	void updateVertices();
	void updateColors();
	void fadeIn(bool textureLoaded);

	unsigned int mColorShift;
	unsigned int mOriginColor;
	bool mColorNotSet;

	std::shared_ptr<TextureResource> mTexture;
	unsigned char mFadeOpacity;
	bool mFading;
	bool mForceLoad;
	bool mDynamic;
};

#endif

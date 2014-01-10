#ifndef _IMAGECOMPONENT_H_
#define _IMAGECOMPONENT_H_

#include "../platform.h"
#include GLHEADER

#include "../GuiComponent.h"
#include <string>
#include <memory>
#include "../resources/TextureResource.h"

class ImageComponent : public GuiComponent
{
public:
	//Creates a new GuiImage at the given location. If given an image, it will be loaded. If maxWidth and/or maxHeight are nonzero, the image will be
	//resized to fit. If only one axis is specified, the other will be set in accordance with the image's aspect ratio. If allowUpscale is false,
	//the image will only be downscaled, never upscaled (the image's size must surpass at least one nonzero bound).
	ImageComponent(Window* window, const Eigen::Vector2f& pos = Eigen::Vector2f::Zero(), const std::string& path = "");
	virtual ~ImageComponent();

	void copyScreen(); //Copy the entire screen into a texture for us to use.
	void setImage(std::string path); //Loads the image at the given filepath.
	void setImage(const char* image, size_t length); //Loads image from memory.
	void setImage(const std::shared_ptr<TextureResource>& texture); //Use an already existing texture.
	void setOrigin(float originX, float originY); //Sets the origin as a percentage of this image (e.g. (0, 0) is top left, (0.5, 0.5) is the center)
	inline void setOrigin(Eigen::Vector2f origin) { setOrigin(origin.x(), origin.y()); }
	void setTiling(bool tile); //Enables or disables tiling. Must be called before loading an image or resizing will be weird.
	void setResize(float width, float height);
	inline void setResize(const Eigen::Vector2f& size) { setResize(size.x(), size.y()); }
	void setMaxSize(float width, float height);
	inline void setMaxSize(const Eigen::Vector2f& size) { setMaxSize(size.x(), size.y()); }
	void setColorShift(unsigned int color);

	void setFlipX(bool flip);
	void setFlipY(bool flip);

	//You can get the rendered size of the ImageComponent with getSize().
	Eigen::Vector2i getTextureSize() const;

	Eigen::Vector2f getCenter() const;

	bool hasImage();

	void render(const Eigen::Affine3f& parentTrans) override;

	virtual void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) override;

private:
	Eigen::Vector2f mTargetSize;
	Eigen::Vector2f mOrigin;

	bool mTiled, mFlipX, mFlipY, mTargetIsMax;

	void resize();
	void buildImageArray(int x, int y, GLfloat* points, GLfloat* texs, float percentageX = 1, float percentageY = 1); //writes 12 GLfloat points and 12 GLfloat texture coordinates to a given array at a given position
	void drawImageArray(GLfloat* points, GLfloat* texs, GLubyte* colors, unsigned int count = 6); //draws the given set of points and texture coordinates, number of coordinate pairs may be specified (default 6)

	unsigned int mColorShift;

	std::shared_ptr<TextureResource> mTexture;
};

#endif

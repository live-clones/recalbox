#pragma once

#include "components/base/Component.h"
#include "resources/TextureResource.h"

// Display an image in a way so that edges don't get too distorted no matter the final size. Useful for UI elements like backgrounds, buttons, etc.
// This is accomplished by splitting an image into 9 pieces:
//  ___________
// |_1_|_2_|_3_|
// |_4_|_5_|_6_|
// |_7_|_8_|_9_|

// Corners (1, 3, 7, 9) will not be stretched at all.
// Borders (2, 4, 6, 8) will be stretched along one axis (2 and 8 horizontally, 4 and 6 vertically).
// The center (5) will be stretched.

class NinePatchComponent : public Component
{
public:
	NinePatchComponent(WindowManager&window, const Path& path);
  explicit NinePatchComponent(WindowManager&window);

	void Render(const Transform4x4f& parentTrans) override;

	void onSizeChanged() override;

	void fitTo(Vector2f size, Vector3f position = Vector3f::Zero(), Vector2f padding = Vector2f::Zero());

	void setImagePath(const Path& path);
	void setEdgeColor(unsigned int edgeColor); // Apply a color shift to the "edge" parts of the ninepatch.
	void setCenterColor(unsigned int centerColor); // Apply a color shift to the "center" part of the ninepatch.

	void applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties) override;

	float MargingX() const { return mW; }
  float MargingY() const { return mH; }

private:
	void buildVertices();
	void updateColors();

	struct Vertex
	{
		Vector2f pos;
		Vector2f tex;
	};

	Vertex mVertices[6 * 9];
	GLubyte mColors[6 * 9 * 4];

	Path mPath;
	float mW;
	float mH;
	unsigned int mEdgeColor;
	unsigned int mCenterColor;
	std::shared_ptr<TextureResource> mTexture;
};

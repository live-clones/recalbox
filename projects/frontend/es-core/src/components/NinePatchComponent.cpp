#include "components/NinePatchComponent.h"
#include "WindowManager.h"
#include "utils/Log.h"
#include "Renderer.h"
#include "themes/ThemeData.h"

NinePatchComponent::NinePatchComponent(WindowManager& window)
  : Component(window)
  , mVertices()
  , mColors()
  , mPath()
  , mW(0)
  , mH(0)
  , mEdgeColor(0xFFFFFFFF)
  , mCenterColor(0xFFFFFFFF)
{
}

NinePatchComponent::NinePatchComponent(WindowManager& window, const Path& path)
	: NinePatchComponent(window)
{
  mPath = path;
	if (!mPath.IsEmpty())
		buildVertices();
}

void NinePatchComponent::updateColors()
{
	Renderer::BuildGLColorArray(mColors, mEdgeColor, 6 * 9);
	Renderer::BuildGLColorArray(&mColors[4 * 6 * 4], mCenterColor, 6);
}

void NinePatchComponent::buildVertices()
{
	mTexture = TextureResource::get(mPath);

	if(mTexture->getSize() == Vector2i::Zero())
	{
    { LOG(LogWarning) << "[NinePatchComponent] missing texture!"; }
		return;
	}

	updateColors();

	const Vector2f ts = mTexture->getSize().toFloat();
	mW = ts.x() / 3.f;
  mH = ts.y() / 3.f;

	//coordinates on the image in pixels, top left origin
	const Vector2f pieceCoords[9] = {
		Vector2f(       0,        0),
		Vector2f(mW * 1.f,        0),
		Vector2f(mW * 2.f,        0),
		Vector2f(       0, mH * 1.f),
		Vector2f(mW * 1.f, mH * 1.f),
		Vector2f(mW * 2.f, mH * 1.f),
		Vector2f(       0, mH * 2.f),
		Vector2f(mW * 1.f, mH * 2.f),
		Vector2f(mW * 2.f, mH * 2.f),
	};

	//corners never stretch, so we calculate a width and height for slices 1, 3, 5, and 7
	float borderWidth = mSize.x() - (mW * 2); //should be pieceSizes[0] and pieceSizes[2]
	//if(borderWidth < pieceSizes.x())
	//	borderWidth = pieceSizes.x();

	float borderHeight = mSize.y() - (mH * 2); //should be pieceSizes[0] and pieceSizes[6]
	//if(borderHeight < pieceSizes.y())
	//	borderHeight = pieceSizes.y();

	mVertices[0 * 6].pos = pieceCoords[0]; //top left
	mVertices[1 * 6].pos = pieceCoords[1]; //top middle
	mVertices[2 * 6].pos = pieceCoords[1] + Vector2f(borderWidth, 0); //top right

	mVertices[3 * 6].pos = mVertices[0 * 6].pos + Vector2f(0, mH); //mid left
	mVertices[4 * 6].pos = mVertices[3 * 6].pos + Vector2f(mW, 0); //mid middle
	mVertices[5 * 6].pos = mVertices[4 * 6].pos + Vector2f(borderWidth, 0); //mid right

	mVertices[6 * 6].pos = mVertices[3 * 6].pos + Vector2f(0, borderHeight); //bot left
	mVertices[7 * 6].pos = mVertices[6 * 6].pos + Vector2f(mW, 0); //bot middle
	mVertices[8 * 6].pos = mVertices[7 * 6].pos + Vector2f(borderWidth, 0); //bot right

	int v = 0;
	for (int slice = 0; slice < 9; slice++)
	{
		Vector2f size(0, 0);

		//corners
		if(slice == 0 || slice == 2 || slice == 6 || slice == 8) size.Set(mW, mH);
		//vertical borders
		if(slice == 1 || slice == 7) size.Set(borderWidth, mH);
		//horizontal borders
		if(slice == 3 || slice == 5) size.Set(mW, borderHeight);

		//center
		if(slice == 4) size.Set(borderWidth, borderHeight);

		//no resizing will be necessary
		//mVertices[v + 0] is already correct
		mVertices[v + 1].pos = mVertices[v + 0].pos + size;
		mVertices[v + 2].pos.Set(mVertices[v + 0].pos.x(), mVertices[v + 1].pos.y());

		mVertices[v + 3].pos.Set(mVertices[v + 1].pos.x(), mVertices[v + 0].pos.y());
		mVertices[v + 4].pos = mVertices[v + 1].pos;
		mVertices[v + 5].pos = mVertices[v + 0].pos;

		//texture coordinates
		//the y = (1 - y) is to deal with texture coordinates having a bottom left corner origin vs. verticies having a top left origin
		mVertices[v + 0].tex.Set(pieceCoords[slice].x() / ts.x(), 1 - (pieceCoords[slice].y() / ts.y()));
		mVertices[v + 1].tex.Set((pieceCoords[slice].x() + mW) / ts.x(), 1 - ((pieceCoords[slice].y() + mH) / ts.y()));
		mVertices[v + 2].tex.Set(mVertices[v + 0].tex.x(), mVertices[v + 1].tex.y());

		mVertices[v + 3].tex.Set(mVertices[v + 1].tex.x(), mVertices[v + 0].tex.y());
		mVertices[v + 4].tex = mVertices[v + 1].tex;
		mVertices[v + 5].tex = mVertices[v + 0].tex;

		v += 6;
	}

	// round vertices
	for(int i = 6*9; --i >= 0; )
	{
		mVertices[i].pos.round();
	}
}

void NinePatchComponent::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = (parentTrans * getTransform()).round();
	
	if(mTexture)
	{
		Renderer::SetMatrix(trans);

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

		glDrawArrays(GL_TRIANGLES, 0, 6 * 9);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	renderChildren(trans);
}

void NinePatchComponent::onSizeChanged()
{
	buildVertices();
}

void NinePatchComponent::fitTo(Vector2f size, Vector3f position, Vector2f padding)
{
	size += padding;
	position[0] -= padding.x() / 2;
	position[1] -= padding.y() / 2;

	setSize(size + Vector2f(mW * 2, mH * 2));
	setPosition(-mW + position.x(), -mH + position.y());
}

void NinePatchComponent::setImagePath(const Path& path)
{
	mPath = path;
	buildVertices();
}

void NinePatchComponent::setEdgeColor(unsigned int edgeColor)
{
	mEdgeColor = edgeColor;
	updateColors();
}

void NinePatchComponent::setCenterColor(unsigned int centerColor)
{
	mCenterColor = centerColor;
	updateColors();
}

void NinePatchComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	Component::applyTheme(theme, view, element, properties);

	const ThemeElement* elem = theme.getElement(view, element, "ninepatch");
	if(elem == nullptr)
		return;

	if(hasFlag(properties, ThemeProperties::Path) && elem->HasProperty("path"))
		setImagePath(Path(elem->AsString("path")));
}

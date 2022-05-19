#include <utils/locale/LocaleHelper.h>
#include <components/RatingComponent.h>
#include <themes/ThemeData.h>
#include "Renderer.h"
#include "WindowManager.h"

RatingComponent::RatingComponent(WindowManager&window, unsigned int color, float value)
  : Component(window)
  , mValue(value)
  , mVertices()
  , mColor(color)
  , mOriginColor(color)
{
	mFilledTexture = TextureResource::get(Path(":/star_filled.svg"), true);
	mUnfilledTexture = TextureResource::get(Path(":/star_unfilled.svg"), true);
	mSize.Set(64 * NUM_RATING_STARS, 64);
	updateVertices();
}

RatingComponent::RatingComponent(WindowManager&window, float value)
  : RatingComponent(window, 0xFFFFFFFF, value)
{
}

void RatingComponent::setValue(float value)
{
  mValue = Math::clamp(value, 0.f, 1.f);
  updateVertices();
}

void RatingComponent::setValue(const std::string& value)
{
  mValue = 0.0f;
	if(!value.empty())
  {
		Strings::ToFloat(value, mValue);
		if(mValue > 1.0f)	mValue = 1.0f;
		else if(mValue < 0.0f) mValue = 0.0f;
	}

	updateVertices();
}

void RatingComponent::onSizeChanged()
{
	if(mSize.y() == 0)

		mSize[1] = mSize.x() / NUM_RATING_STARS;
	else if(mSize.x() == 0)
		mSize[0] = mSize.y() * NUM_RATING_STARS;

	if(mSize.y() > 0)
	{
		size_t heightPx = (size_t)Math::roundi(mSize.y());
		if (mFilledTexture)
			mFilledTexture->rasterizeAt(heightPx, heightPx);
		if(mUnfilledTexture)
			mUnfilledTexture->rasterizeAt(heightPx, heightPx);
	}

	updateVertices();
}

void RatingComponent::updateVertices()
{
	const float numStars = NUM_RATING_STARS;

	const float h = Math::round(getSize().y()); // is the same as a single star's width
	const float w = Math::round(h * mValue * numStars);
	const float fw = Math::round(h * numStars);

	mVertices[0].pos.Set(0.0f, 0.0f);
	mVertices[0].tex.Set(0.0f, 1.0f);
	mVertices[1].pos.Set(w, h);
	mVertices[1].tex.Set(mValue * numStars, 0.0f);
	mVertices[2].pos.Set(0.0f, h);
	mVertices[2].tex.Set(0.0f, 0.0f);

	mVertices[3] = mVertices[0];
	mVertices[4].pos.Set(w, 0.0f);
	mVertices[4].tex.Set(mValue * numStars, 1.0f);
	mVertices[5] = mVertices[1];

	mVertices[6] = mVertices[4];
	mVertices[7].pos.Set(fw, h);
	mVertices[7].tex.Set(numStars, 0.0f);
	mVertices[8] = mVertices[1];

	mVertices[9] = mVertices[6];
	mVertices[10].pos.Set(fw, 0.0f);
	mVertices[10].tex.Set(numStars, 1.0f);
	mVertices[11] = mVertices[7];
}

void RatingComponent::Render(const Transform4x4f& parentTrans)
{
    if(mThemeDisabled)
    {
        return;
    }

	Transform4x4f trans = (parentTrans * getTransform()).round();
	Renderer::SetMatrix(trans);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	int r = ((int)mColor >> 24) & 0xFF;
	int g = ((int)mColor >> 16) & 0xFF;
	int b = ((int)mColor >> 8 ) & 0xFF;
	//int a = mColor & 0xFF;

	glColor4ub(r, g, b, getOpacity());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &mVertices[0].pos);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &mVertices[0].tex);
	
	mFilledTexture->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	mUnfilledTexture->bind();
	glDrawArrays(GL_TRIANGLES, 6, 6);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glColor4ub(255, 255, 255, 255);

	renderChildren(trans);
}

bool RatingComponent::ProcessInput(const InputCompactEvent& event)
{
	if (event.ValidPressed() || event.AnyRightPressed())
	{
		if((mValue += .5f / NUM_RATING_STARS) > 1.1f) mValue = 0.f;
		updateVertices();
		if (mInterface != nullptr)
		  mInterface->RatingChanged(mId, mValue);
	}
	else if (event.AnyLeftPressed())
  {
    if((mValue -= .5f / NUM_RATING_STARS) < 0.f) mValue = 1.f;
    updateVertices();
    if (mInterface != nullptr)
      mInterface->RatingChanged(mId, mValue);
  }

	return Component::ProcessInput(event);
}

void RatingComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	Component::applyTheme(theme, view, element, properties);

	const ThemeElement* elem = theme.getElement(view, element, "rating");
	if (elem == nullptr)
		return;

	bool imgChanged = false;
	if (hasFlag(properties, ThemeProperties::Path) && elem->HasProperty("filledPath"))
	{
		mFilledTexture = TextureResource::get(Path(elem->AsString("filledPath")), true);
		imgChanged = true;
	}
	if (hasFlag(properties, ThemeProperties::Path) && elem->HasProperty("unfilledPath"))
	{
		mUnfilledTexture = TextureResource::get(Path(elem->AsString("unfilledPath")), true);
		imgChanged = true;
	}

	if(imgChanged)
		onSizeChanged();
}

bool RatingComponent::getHelpPrompts(Help& help)
{
	help.Set(Help::Valid(), _("ADD STAR"));
	return true;
}


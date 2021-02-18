#include <utils/Strings.h>
#include "components/TextComponent.h"
#include "Renderer.h"
#include "utils/Log.h"
#include "WindowManager.h"
#include "themes/ThemeData.h"

TextComponent::TextComponent(WindowManager&window)
	: Component(window),
    mFont(Font::get(FONT_SIZE_MEDIUM)),
    mColor(0x000000FF),
    mOriginColor(0x000000FF),
    mBgColor(0),
    mLineSpacing(1.5f),
    mColorOpacity(0xFF),
    mBgColorOpacity(0),
    mHorizontalAlignment(TextAlignment::Left),
    mVerticalAlignment(TextAlignment::Center),
    mRenderBackground(false),
    mUppercase(false),
    mAutoCalcExtentX(true),
    mAutoCalcExtentY(true)
{
  mPosition = Vector3f::Zero();
  mSize = Vector2f::Zero();
}
TextComponent::TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color)
  : TextComponent(window)
{
  mFont = font;
  mText = text;
  mColor = color;
  mColorOpacity = (unsigned char)(color & 0xFF);
  mOriginColor = color;
  onTextChanged();
}

TextComponent::TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align)
  : TextComponent(window)
{
  mFont = font;
  mText = text;
  mColor = color;
  mColorOpacity = (unsigned char)(color & 0xFF);
  mOriginColor = color;
  mHorizontalAlignment = align;
  onTextChanged();
}

TextComponent::TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align,
                             Vector3f pos, Vector2f size, unsigned int bgcolor)
	: TextComponent(window, text, font, color, align)
{
  mBgColor = bgcolor;
  mBgColorOpacity = (unsigned char)(bgcolor & 0xFF);
  mPosition = pos;
  mSize = size;
}

void TextComponent::onSizeChanged()
{
	mAutoCalcExtentX = (getSize().x() == 0);
	mAutoCalcExtentY = (getSize().y() == 0);
	onTextChanged();
}

void TextComponent::setFont(const std::shared_ptr<Font>& font)
{
	mFont = font;
	onTextChanged();
}

void TextComponent::setColor(unsigned int color)
{
    mColor = color;
    unsigned char opacity = mColor & 0x000000FF;
    Component::setOpacity(opacity);

    mColorOpacity = mColor & 0x000000FF;

    onColorChanged();
}

//  Set the color of the background box
void TextComponent::setBackgroundColor(unsigned int color)
{
	mBgColor = color;
	mBgColorOpacity = mBgColor & 0x000000FF;
}


//  Scale the opacity
void TextComponent::setOpacity(unsigned char opacity)
{
	// This method is mostly called to do fading in-out of the Text component element.
	// Therefore, we assume here that opacity is a fractional value (expressed as an int 0-255),
	// of the opacity originally set with setColor() or setBackgroundColor().
 
	unsigned char o = (unsigned char)((float)opacity / 255.f * (float) mColorOpacity);
	mColor = (mColor & 0xFFFFFF00) | (unsigned char) o;

	unsigned char bgo = (unsigned char)((float)opacity / 255.f * (float)mBgColorOpacity);
	mBgColor = (mBgColor & 0xFFFFFF00) | (unsigned char)bgo;

	onColorChanged();

	Component::setOpacity(opacity);
}

void TextComponent::setText(const std::string& text)
{
  mText = mUppercase ? Strings::ToUpperUTF8(text) : text;
	onTextChanged();
}

void TextComponent::setUppercase(bool uppercase)
{
	mUppercase = uppercase;
	onTextChanged();
}

void TextComponent::Render(const Transform4x4f& parentTrans)
{
    if(mDisabled)
    {
        return;
    }
	Transform4x4f trans = parentTrans * getTransform();

	if (mRenderBackground)
	{
		Renderer::SetMatrix(trans);
		Renderer::DrawRectangle(0.f, 0.f, mSize.x(), mSize.y(), mBgColor);
	}

	if(mTextCache)
	{
		const Vector2f& textSize = mTextCache->metrics.size;
		float yOff = 0;
		switch(mVerticalAlignment)
			{
			case TextAlignment::Top:
				yOff = 0;
				break;
			case TextAlignment::Bottom:
				yOff = (getSize().y() - textSize.y());
				break;
			case TextAlignment::Center:
				yOff = (getSize().y() - textSize.y()) / 2.0f;
				break;
				case TextAlignment::Left:
				case TextAlignment::Right: break;
			}
		Vector3f off(0, yOff, 0);

		if (false /*Settings::Instance().DebugText()*/)
		{
			// draw the "textbox" area, what we are aligned within
			Renderer::SetMatrix(trans);
			Renderer::DrawRectangle(0.f, 0.f, mSize.x(), mSize.y(), 0xFF000033);
		}
		
		trans.translate(off);
		trans.round();
		Renderer::SetMatrix(trans);

		// draw the text area, where the text actually is going
		if (false /*Settings::Instance().DebugText()*/)
		{
			switch(mHorizontalAlignment)
			{
			case TextAlignment::Left:
				Renderer::DrawRectangle(0.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case TextAlignment::Center:
				Renderer::DrawRectangle((mSize.x() - mTextCache->metrics.size.x()) / 2.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case TextAlignment::Right:
				Renderer::DrawRectangle(mSize.x() - mTextCache->metrics.size.x(), 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
        case TextAlignment::Top:
        case TextAlignment::Bottom:break;
      }
		}

		mFont->renderTextCache(mTextCache.get());
	}

}

void TextComponent::calculateExtent()
{
	if(mAutoCalcExtentX)
	{
		mSize = mFont->sizeText(mText, mLineSpacing);
	}else{
		if(mAutoCalcExtentY)
		{
			mSize[1] = mFont->sizeWrappedText(mText, getSize().x(), mLineSpacing).y();
		}
	}
}

void TextComponent::onTextChanged()
{
	calculateExtent();

	if(!mFont || mText.empty())
	{
		mTextCache.reset();
		return;
	}

	std::string text = mUppercase ? Strings::ToUpperUTF8(mText) : mText;

	std::shared_ptr<Font> f = mFont;
	const bool isMultiline = (mSize.y() == 0 || mSize.y() > f->getHeight()*1.2f);

	bool addAbbrev = false;
	if (!isMultiline)
	{
		size_t newline = text.find('\n');
		text = text.substr(0, newline); // single line of text - stop at the first newline since it'll mess everything up
		addAbbrev = newline != std::string::npos;
	}

	Vector2f size = f->sizeText(text);
	if(!isMultiline && (mSize.x() != 0) && !text.empty() && (size.x() > mSize.x() || addAbbrev))
	{
		// abbreviate text
		const std::string abbrev = "...";
		Vector2f abbrevSize = f->sizeText(abbrev);

		while(!text.empty() && size.x() + abbrevSize.x() > mSize.x())
		{
			size_t newSize = Font::getPrevCursor(text, text.size());
			text.erase(newSize, text.size() - newSize);
			size = f->sizeText(text);
		}

		text.append(abbrev);

		mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(text, Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, mSize.x(), mHorizontalAlignment, mLineSpacing));
	}else{
		mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(f->wrapText(text, mSize.x()), Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, mSize.x(), mHorizontalAlignment, mLineSpacing));
	}
}

void TextComponent::onColorChanged()
{
	if(mTextCache)
	{
		mTextCache->setColor(mColor);
	}
}

void TextComponent::setHorizontalAlignment(TextAlignment align)
{
	mHorizontalAlignment = align;
	onTextChanged();
}

void TextComponent::setLineSpacing(float spacing)
{
	mLineSpacing = spacing;
	onTextChanged();
}

void TextComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	Component::applyTheme(theme, view, element, properties);

	const ThemeElement* elem = theme.getElement(view, element, "text");
	if(elem == nullptr)
		return;

	if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("color"))
		setColor((unsigned int)elem->AsInt("color"));

	setRenderBackground(false);
	if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("backgroundColor")) {
		setBackgroundColor((unsigned int)elem->AsInt("backgroundColor"));
		setRenderBackground(true);
	}

	if(hasFlag(properties, ThemeProperties::Alignment) && elem->HasProperty("alignment"))
	{
		std::string str = elem->AsString("alignment");
		if(str == "left")
			setHorizontalAlignment(TextAlignment::Left);
		else if(str == "center")
			setHorizontalAlignment(TextAlignment::Center);
		else if(str == "right")
			setHorizontalAlignment(TextAlignment::Right);
		else
    { LOG(LogError) << "[TextComponent] Unknown text alignment string: " << str; }
	}

	if (hasFlag(properties, ThemeProperties::Text) && elem->HasProperty("text"))
		setText(elem->AsString("text"));

	if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->HasProperty("forceUppercase"))
		setUppercase(elem->AsBool("forceUppercase"));

	if (hasFlag(properties, ThemeProperties::LineSpacing) && elem->HasProperty("lineSpacing"))
		setLineSpacing(elem->AsFloat("lineSpacing"));

	setFont(Font::getFromTheme(elem, properties, mFont));
}

#include <utils/StringUtil.h>
#include "components/TextComponent.h"
#include "Renderer.h"
#include "Log.h"
#include "Window.h"
#include "themes/ThemeData.h"
#include "Util.h"
#include "Settings.h"
#include "Locale.h"

TextComponent::TextComponent(Window* window)
	: GuiComponent(window),
		mFont(Font::get(FONT_SIZE_MEDIUM)),
    mColor(0x000000FF),
    mOriginColor(0x000000FF),
    mBgColor(0),
		mHorizontalAlignment(TextAlignment::Left),
		mVerticalAlignment(TextAlignment::Center),
		mLineSpacing(1.5f),
    mRenderBackground(false),
    mUppercase(false),
    mAutoCalcExtentX(true),
    mAutoCalcExtentY(true)
{
}

TextComponent::TextComponent(Window* window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align,
                             Vector3f pos, Vector2f size, unsigned int bgcolor)
	: GuiComponent(window),
	  mFont(nullptr),
    mColor(0x000000FF),
    mOriginColor(0x000000FF),
    mBgColor(0),
		mHorizontalAlignment(align),
		mVerticalAlignment(TextAlignment::Center),
		mLineSpacing(1.5f),
    mRenderBackground(false),
    mUppercase(false),
    mAutoCalcExtentX(true),
    mAutoCalcExtentY(true)
{
  setFont(font);
	setColor(color);
	setOriginColor(color);
	setBackgroundColor(bgcolor);
	setText(text);
	setPosition(pos);
	setSize(size);
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
    GuiComponent::setOpacity(opacity);

    mColorOpacity = mColor & 0x000000FF;

    onColorChanged();
}

unsigned int TextComponent::getOriginColor()
{
	return mOriginColor;
}

//  Set the color of the background box
void TextComponent::setBackgroundColor(unsigned int color)
{
	mBgColor = color;
	mBgColorOpacity = mBgColor & 0x000000FF;
}

void TextComponent::setRenderBackground(bool render)
{
	mRenderBackground = render;
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

	GuiComponent::setOpacity(opacity);
}

unsigned char TextComponent::getOpacity() const
{
	return mColor & 0x000000FF;
}

void TextComponent::setText(const std::string& text)
{
        mText = text;
	onTextChanged();
}

void TextComponent::setUppercase(bool uppercase)
{
	mUppercase = uppercase;
	onTextChanged();
}

void TextComponent::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	if (mRenderBackground)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), mBgColor);
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
				case TextAlignment::Left: break;
				case TextAlignment::Right: break;
			}
		Vector3f off(0, yOff, 0);

		if(Settings::getInstance()->getBool("DebugText"))
		{
			// draw the "textbox" area, what we are aligned within
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0xFF000033);
		}
		
		trans.translate(off);
		trans = roundMatrix(trans);
		Renderer::setMatrix(trans);

		// draw the text area, where the text actually is going
		if(Settings::getInstance()->getBool("DebugText"))
		{
			switch(mHorizontalAlignment)
			{
			case TextAlignment::Left:
				Renderer::drawRect(0.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case TextAlignment::Center:
				Renderer::drawRect((mSize.x() - mTextCache->metrics.size.x()) / 2.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case TextAlignment::Right:
				Renderer::drawRect(mSize.x() - mTextCache->metrics.size.x(), 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
        case TextAlignment::Top:break;
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
		mSize = mFont->sizeText(mUppercase ? StringUtil::toUpper(mText) : mText, mLineSpacing);
	}else{
		if(mAutoCalcExtentY)
		{
			mSize[1] = mFont->sizeWrappedText(mUppercase ? StringUtil::toUpper(mText) : mText, getSize().x(), mLineSpacing).y();
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

	std::string text = mUppercase ? StringUtil::toUpper(mText) : mText;

	std::shared_ptr<Font> f = mFont;
	const bool isMultiline = (mSize.y() == 0 || mSize.y() > f->getHeight()*1.2f);

	bool addAbbrev = false;
	if(!isMultiline)
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

void TextComponent::setVerticalAlignment(TextAlignment align)
{
	mVerticalAlignment = align;
}

void TextComponent::setLineSpacing(float spacing)
{
	mLineSpacing = spacing;
	onTextChanged();
}

void TextComponent::setValue(const std::string& value)
{
	setText(value);
}

std::string TextComponent::getValue() const
{
	return mText;
}

void TextComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	GuiComponent::applyTheme(theme, view, element, properties);

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "text");
	if(!elem)
		return;

	if (hasFlag(properties, ThemeProperties::Color) && elem->has("color"))
		setColor(elem->get<unsigned int>("color"));	

	setRenderBackground(false);
	if (hasFlag(properties, ThemeProperties::Color) && elem->has("backgroundColor")) {
		setBackgroundColor(elem->get<unsigned int>("backgroundColor"));
		setRenderBackground(true);
	}

	if(hasFlag(properties, ThemeProperties::Alignment) && elem->has("alignment"))
	{
		std::string str = elem->get<std::string>("alignment");
		if(str == "left")
			setHorizontalAlignment(TextAlignment::Left);
		else if(str == "center")
			setHorizontalAlignment(TextAlignment::Center);
		else if(str == "right")
			setHorizontalAlignment(TextAlignment::Right);
		else
			LOG(LogError) << "Unknown text alignment string: " << str;
	}

	if (hasFlag(properties, ThemeProperties::Text) && elem->has("text"))
		setText(elem->get<std::string>("text"));

	if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->has("forceUppercase"))
		setUppercase(elem->get<bool>("forceUppercase"));

	if (hasFlag(properties, ThemeProperties::LineSpacing) && elem->has("lineSpacing"))
		setLineSpacing(elem->get<float>("lineSpacing"));

	setFont(Font::getFromTheme(elem, properties, mFont));
}

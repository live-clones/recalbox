#pragma once

#include "components/IList.h"
#include "Renderer.h"
#include "resources/Font.h"
#include "Sound.h"
#include "Log.h"
#include "themes/ThemeData.h"
#include <string>
#include <memory>
#include <functional>
#include <utils/StringUtil.h>

struct TextListData
{
	unsigned int colorId;
	std::shared_ptr<TextCache> textCache;
};

//A graphical list. Supports multiple colors for rows and scrolling.
template <typename T>
class TextListComponent : public IList<TextListData, T>
{
protected:
	using IList<TextListData, T>::mEntries;
	using IList<TextListData, T>::listUpdate;
	using IList<TextListData, T>::listInput;
	using IList<TextListData, T>::listRenderTitleOverlay;
	using IList<TextListData, T>::getTransform;
	using IList<TextListData, T>::mSize;
	using IList<TextListData, T>::mCursor;
  using typename IList<TextListData, T>::Entry;

public:
	using IList<TextListData, T>::size;
	using IList<TextListData, T>::isScrolling;
	using IList<TextListData, T>::stopScrolling;

	explicit TextListComponent(Window* window);
	
	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Transform4x4f& parentTrans) override;
	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, ThemeProperties properties) override;

	void add(const std::string& name, const T& obj, unsigned int colorId, bool toTheBeginning = false);
	
	enum class Alignment
	{
		Left,
		Center,
		Right
	};

	inline void setAlignment(Alignment align) { mAlignment = align; }

	inline void setCursorChangedCallback(const std::function<void(CursorState state)>& func) { mCursorChangedCallback = func; }

	inline void setFont(const std::shared_ptr<Font>& font)
	{
		mFont = font;
		for (auto& entry : mEntries)
			entry.data.textCache.reset();
	}

	inline void setUppercase(bool uppercase) 
	{
		(void)uppercase;
		mUppercase = true; // TODO: Check
		for (auto& entry : mEntries)
			entry.data.textCache.reset();
	}

	inline void setSelectorHeight(float selectorScale) { mSelectorHeight = selectorScale; }
	inline void setSelectorOffsetY(float selectorOffsetY) { mSelectorOffsetY = selectorOffsetY; }
	inline void setSelectorColor(unsigned int color) { mSelectorColor = color; }
	inline void setSelectedColor(unsigned int color) { mSelectedColor = color; }
	inline void setScrollSound(const std::shared_ptr<Sound>& sound) { mScrollSound = sound; }
	inline void setColor(unsigned int id, unsigned int color) { mColors[id] = color; }
	inline void setSound(const std::shared_ptr<Sound>& sound) { mScrollSound = sound; }
	inline void setLineSpacing(float lineSpacing) { mLineSpacing = lineSpacing; }

protected:
	virtual void onScroll(int amt) { (void)amt; if(mScrollSound) mScrollSound->play(); }
	virtual void onCursorChanged(const CursorState& state);

private:
	static const int MARQUEE_DELAY = 2000;
	static const int MARQUEE_SPEED = 8;
	static const int MARQUEE_RATE = 1;

	int mMarqueeOffset;
	int mMarqueeTime;

	Alignment mAlignment;
	float mHorizontalMargin;

	std::function<void(CursorState state)> mCursorChangedCallback;

	std::shared_ptr<Font> mFont;
	bool mUppercase;
	float mLineSpacing;
	float mSelectorHeight;
	float mSelectorOffsetY;
	unsigned int mSelectorColor;
	unsigned int mSelectedColor;
	std::shared_ptr<Sound> mScrollSound;
	static const unsigned int COLOR_ID_COUNT = 2;
	unsigned int mColors[COLOR_ID_COUNT];

	ImageComponent mSelectorImage;
};

template <typename T>
TextListComponent<T>::TextListComponent(Window* window)
  :	IList<TextListData, T>(window),
    mMarqueeOffset(0),
    mMarqueeTime(-MARQUEE_DELAY),
    mAlignment(Alignment::Center),
    mHorizontalMargin(0),
    mFont(Font::get(FONT_SIZE_MEDIUM)),
    mUppercase(false),
    mLineSpacing(1.5f),
    mSelectorOffsetY(0),
    mSelectorColor(0x000000FF),
    mSelectedColor(0),
    mColors{ 0x0000FFFF, 0x00FF00FF},
    mSelectorImage(window)
{
	mSelectorHeight = mFont->getSize() * 1.5f;
}

template <typename T>
void TextListComponent<T>::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();
	
	std::shared_ptr<Font>& font = mFont;

	if(size() == 0)
		return;

	const float entrySize = font->getSize() * mLineSpacing;

	int startEntry = 0;

	//number of entries that can fit on the screen simultaniously
	int screenCount = (int)(mSize.y() / entrySize + 0.5f);
	
	if(size() >= screenCount)
	{
		startEntry = mCursor - screenCount/2;
		if(startEntry < 0)
			startEntry = 0;
		if(startEntry >= size() - screenCount)
			startEntry = size() - screenCount;
	}

	float y = 0;

	int listCutoff = startEntry + screenCount;
	if(listCutoff > size())
		listCutoff = size();

	// draw selector bar
	if(startEntry < listCutoff)
	{
		if (mSelectorImage.hasImage()) {
			mSelectorImage.setPosition(0.f, (mCursor - startEntry)*entrySize + mSelectorOffsetY, 0.f);
			mSelectorImage.render(trans);
		} else {
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.f, (mCursor - startEntry)*entrySize + mSelectorOffsetY, mSize.x(), mSelectorHeight, mSelectorColor);
		}
	}

	// clip to inside margins
	Vector3f dim(mSize.x(), mSize.y(), 0);
	dim = trans * dim - trans.translation();
	Renderer::pushClipRect(Vector2i((int)(trans.translation().x() + mHorizontalMargin), (int)trans.translation().y()),
		Vector2i((int)(dim.x() - mHorizontalMargin*2), (int)dim.y()));

	for (int i = startEntry; i < listCutoff; i++)
	{
		typename IList<TextListData, T>::Entry& entry = mEntries.at((unsigned int)i);

		unsigned int color;
		if(mCursor == i && mSelectedColor)
			color = mSelectedColor;
		else
			color = mColors[entry.data.colorId];

		if(!entry.data.textCache)
			entry.data.textCache = std::unique_ptr<TextCache>(font->buildTextCache(mUppercase ? StringUtil::toUpper(entry.name) : entry.name, 0, 0, 0x000000FF));

		entry.data.textCache->setColor(color);

		Vector3f offset(0, y, 0);

		switch(mAlignment)
		{
		case Alignment::Left:
			offset[0] = mHorizontalMargin;
			break;
		case Alignment::Center:
			offset[0] = (mSize.x() - entry.data.textCache->metrics.size.x()) / 2;
			if(offset[0] < mHorizontalMargin)
				offset[0] = mHorizontalMargin;
			break;
		case Alignment::Right:
			offset[0] = (mSize.x() - entry.data.textCache->metrics.size.x());
			offset[0] -= mHorizontalMargin;
			if(offset[0] < mHorizontalMargin)
				offset[0] = mHorizontalMargin;
			break;
		  default : break;
		}
		
		if(mCursor == i)
			offset[0] -= mMarqueeOffset;
		
		Transform4x4f drawTrans = trans;
		drawTrans.translate(offset);
		Renderer::setMatrix(drawTrans);

		font->renderTextCache(entry.data.textCache.get());
		
		y += entrySize;
	}

	Renderer::popClipRect();

	listRenderTitleOverlay(trans);

	GuiComponent::renderChildren(trans);
}

template <typename T>
bool TextListComponent<T>::input(InputConfig* config, Input input)
{
	if(size() > 0)
	{
		if(input.value != 0)
		{
			if(config->isMappedTo("down", input))
			{
				listInput(1);
				return true;
			}

			if(config->isMappedTo("up", input))
			{
				listInput(-1);
				return true;
			}
			if(config->isMappedTo("pagedown", input))
			{
				listInput(10);
				return true;
			}

			if(config->isMappedTo("pageup", input))
			{
				listInput(-10);
				return true;
			}
		}else{
			if(config->isMappedTo("down", input) || config->isMappedTo("up", input) || 
				config->isMappedTo("pagedown", input) || config->isMappedTo("pageup", input))
			{
				stopScrolling();
			}
		}
	}

	return GuiComponent::input(config, input);
}

template <typename T>
void TextListComponent<T>::update(int deltaTime)
{
	listUpdate(deltaTime);
	if(!isScrolling() && size() > 0)
	{
		//if we're not scrolling and this object's text goes outside our size, marquee it!
		const std::string& text = mEntries.at((unsigned int)mCursor).name;

		Vector2f textSize = mFont->sizeText(text);

		//it's long enough to marquee
		if(textSize.x() - mMarqueeOffset > mSize.x() - 12 - mHorizontalMargin * 2)
		{
			mMarqueeTime += deltaTime;
			while(mMarqueeTime > MARQUEE_SPEED)
			{
				mMarqueeOffset += MARQUEE_RATE;
				mMarqueeTime -= MARQUEE_SPEED;
			}
		}
	}

	GuiComponent::update(deltaTime);
}

//list management stuff
template <typename T>
void TextListComponent<T>::add(const std::string& name, const T& obj, unsigned int color, bool toTheBeginning)
{
	assert(color < COLOR_ID_COUNT);

	typename IList<TextListData, T>::Entry entry;
	entry.name = name;
	entry.object = obj;
	entry.data.colorId = color;
	if (toTheBeginning) {
    ((IList< TextListData, T >*)this)->unshift(entry);
	} else {
    ((IList< TextListData, T >*)this)->add(entry);
	}
}

template <typename T>
void TextListComponent<T>::onCursorChanged(const CursorState& state)
{
	mMarqueeOffset = 0;
	mMarqueeTime = -MARQUEE_DELAY;

	if(mCursorChangedCallback)
		mCursorChangedCallback(state);
}

template <typename T>
void TextListComponent<T>::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	GuiComponent::applyTheme(theme, view, element, properties);

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "textlist");
	if (elem == nullptr)
		return;

	if (hasFlag(properties, ThemeProperties::Color))
	{
		if(elem->has("selectorColor"))
			setSelectorColor(elem->get<unsigned int>("selectorColor"));
		if(elem->has("selectedColor"))
			setSelectedColor(elem->get<unsigned int>("selectedColor"));
		if(elem->has("primaryColor"))
			setColor(0, elem->get<unsigned int>("primaryColor"));
		if(elem->has("secondaryColor"))
			setColor(1, elem->get<unsigned int>("secondaryColor"));
	}

	setFont(Font::getFromTheme(elem, properties, mFont));
	
	if (hasFlag(properties, ThemeProperties::Sound) && elem->has("scrollSound"))
		setSound(Sound::get(elem->get<std::string>("scrollSound")));

	if (hasFlag(properties, ThemeProperties::Alignment))
	{
		if(elem->has("alignment"))
		{
			const std::string& str = elem->get<std::string>("alignment");
			if(str == "left")
				setAlignment(Alignment::Left);
			else if(str == "center")
				setAlignment(Alignment::Center);
			else if(str == "right")
				setAlignment(Alignment::Right);
			else
				LOG(LogError) << "Unknown TextListComponent alignment \"" << str << "\"!";
		}
		if(elem->has("horizontalMargin"))
		{
			mHorizontalMargin = elem->get<float>("horizontalMargin") * (this->mParent ? this->mParent->getSize().x() : Renderer::getDisplayWidthAsFloat());
		}
	}

	if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->has("forceUppercase"))
		setUppercase(elem->get<bool>("forceUppercase"));

	if (hasFlag(properties, ThemeProperties::LineSpacing))
	{
		if(elem->has("lineSpacing"))
			setLineSpacing(elem->get<float>("lineSpacing"));
		if(elem->has("selectorHeight"))
		{
			setSelectorHeight(elem->get<float>("selectorHeight") * Renderer::getDisplayHeightAsFloat());
		} else {
			setSelectorHeight(mFont->getSize() * 1.5);
		}
		if(elem->has("selectorOffsetY"))
		{
			float scale = this->mParent ? this->mParent->getSize().y() : Renderer::getDisplayHeightAsFloat();
			setSelectorOffsetY(elem->get<float>("selectorOffsetY") * scale);
		} else {
			setSelectorOffsetY(0.0);
		}
	}

	if (elem->has("selectorImagePath"))
	{
		std::string path = elem->get<std::string>("selectorImagePath");
		bool tile = elem->has("selectorImageTile") && elem->get<bool>("selectorImageTile");
		mSelectorImage.setImage(path, tile);
		mSelectorImage.setSize(mSize.x(), mSelectorHeight);
		mSelectorImage.setColorShift(mSelectorColor);
	} else {
		mSelectorImage.setImage("");
	}
}

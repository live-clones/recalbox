#pragma once

#include "components/IList.h"
#include "Renderer.h"
#include "resources/Font.h"
#include "utils/Log.h"
#include "themes/ThemeData.h"
#include <string>
#include <memory>
#include <functional>
#include <utils/Strings.h>
#include <audio/AudioManager.h>

enum class HorizontalAlignment : char
{
    Left,
    Center,
    Right
};

struct TextListData
{
  std::shared_ptr<TextCache> textCache;
	signed char colorId;
  signed char colorBackgroundId;
  HorizontalAlignment hzAlignement;
  bool useHzAlignment;
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

	explicit TextListComponent(Window& window);
	
	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	void applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties) override;

	void add(const std::string& name, const T& obj, signed char colorId, bool toTheBeginning = false);
  void add(const std::string& name, const T& obj, signed char colorId, signed char colorBackgroundId, HorizontalAlignment alignment);
  void changeTextAt(int index, const std::string& name);
  void changeBackgroundColorAt(int index, int colorIndex);

  inline void setSelectedAt(int index, const T& object) { mEntries.at(index).object = object; }
	inline void setAlignment(HorizontalAlignment align) { mAlignment = align; }
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
	inline void setScrollSound(AudioManager::AudioHandle sound) { mScrollSound = sound; }
	inline void setColor(unsigned int id, unsigned int color) { mColors[id] = color; }
	inline void setSound(AudioManager::AudioHandle sound) { mScrollSound = sound; }
	inline void setLineSpacing(float lineSpacing) { mLineSpacing = lineSpacing; }
  inline void setHorizontalMargin(float horizontalMargin) { mHorizontalMargin = horizontalMargin; }

protected:
	virtual void onScroll(int amt) { (void)amt; AudioManager::Instance().PlaySound(mScrollSound); }
	virtual void onCursorChanged(const CursorState& state);

private:
	static const int MARQUEE_DELAY = 2000;
	static const int MARQUEE_SPEED = 8;
	static const int MARQUEE_RATE = 1;

	int mMarqueeOffset;
	int mMarqueeTime;

	HorizontalAlignment mAlignment;
	float mHorizontalMargin;

	std::function<void(CursorState state)> mCursorChangedCallback;

	std::shared_ptr<Font> mFont;
	bool mUppercase;
	float mLineSpacing;
	float mSelectorHeight;
	float mSelectorOffsetY;
	unsigned int mSelectorColor;
	unsigned int mSelectedColor;
	AudioManager::AudioHandle mScrollSound;
	static constexpr unsigned int COLOR_ID_COUNT = 8;
	unsigned int mColors[COLOR_ID_COUNT];

	ImageComponent mSelectorImage;
};

template <typename T>
TextListComponent<T>::TextListComponent(Window& window)
  :	IList<TextListData, T>(window),
    mMarqueeOffset(0),
    mMarqueeTime(-MARQUEE_DELAY),
    mAlignment(HorizontalAlignment::Center),
    mHorizontalMargin(0),
    mFont(Font::get(FONT_SIZE_MEDIUM)),
    mUppercase(false),
    mLineSpacing(1.5f),
    mSelectorOffsetY(0),
    mSelectorColor(0x000000FF),
    mSelectedColor(0),
    mScrollSound(0),
    mColors{ 0x0000FFFF, 0x00FF00FF},
    mSelectorImage(window)
{
	mSelectorHeight = mFont->getSize() * 1.5f;
}

template <typename T>
void TextListComponent<T>::Render(const Transform4x4f& parentTrans)
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
	if(listCutoff > size())	listCutoff = size();

  // clip to inside margins
  Vector3f dim(mSize.x(), mSize.y(), 0);
  dim = trans * dim - trans.translation();
  Renderer::pushClipRect(Vector2i((int)(trans.translation().x()/* + mHorizontalMargin*/), (int)trans.translation().y()),
                         Vector2i((int)(dim.x() /*- mHorizontalMargin*/), (int)dim.y()));

  // draw selector bar
	if(startEntry < listCutoff)
	{
		if (mSelectorImage.hasImage()) {
			mSelectorImage.setPosition(0.f, (mCursor - startEntry)*entrySize + mSelectorOffsetY, 0.f);
      mSelectorImage.Render(trans);
		} else {
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.f, (mCursor - startEntry)*entrySize + mSelectorOffsetY, mSize.x(), mSelectorHeight, mSelectorColor);
		}
	}

	for (int i = startEntry; i < listCutoff; i++)
	{
		typename IList<TextListData, T>::Entry& entry = mEntries.at((unsigned int)i);

		if ((unsigned int)entry.data.colorBackgroundId < COLOR_ID_COUNT)
    {
      Renderer::setMatrix(trans);
      Renderer::drawRect(0.f, (float)(i - startEntry) * entrySize + mSelectorOffsetY, mSize.x(), mSelectorHeight,
                         mColors[entry.data.colorBackgroundId]);
    }

    unsigned int color;
		if(mCursor == i && mSelectedColor)
			color = mSelectedColor;
		else
			color = mColors[entry.data.colorId];

		if(!entry.data.textCache)
			entry.data.textCache = std::unique_ptr<TextCache>(font->buildTextCache(mUppercase ? Strings::ToUpperUTF8(
        entry.name) : entry.name, 0, 0, 0x000000FF));

		entry.data.textCache->setColor(color);

		Vector3f offset(0, y, 0);

		switch(entry.data.useHzAlignment ? entry.data.hzAlignement : mAlignment)
		{
		case HorizontalAlignment::Left:
			offset[0] = mHorizontalMargin;
			break;
		case HorizontalAlignment::Center:
			offset[0] = (mSize.x() - entry.data.textCache->metrics.size.x()) / 2;
			if(offset[0] < mHorizontalMargin)
				offset[0] = mHorizontalMargin;
			break;
		case HorizontalAlignment::Right:
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

	Component::renderChildren(trans);
}

template <typename T>
bool TextListComponent<T>::ProcessInput(const InputCompactEvent& event)
{
	if(size() > 0)
	{
    if (event.AnyDownPressed())
    {
      listInput(1);
      return true;
    }
    else if (event.AnyUpPressed())
    {
      listInput(-1);
      return true;
    }
    else if (event.R1Pressed())
    {
      listInput(10);
      return true;
    }
    else if (event.L1Pressed())
    {
      listInput(-10);
      return true;
    }
    else if (event.AnyDownReleased() || event.AnyUpReleased() || event.R1Released() || event.L1Released())
    {
      stopScrolling();
    }
	}

	return Component::ProcessInput(event);
}

template <typename T>
void TextListComponent<T>::Update(int deltaTime)
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

  Component::Update(deltaTime);
}

//list management stuff
template <typename T>
void TextListComponent<T>::add(const std::string& name, const T& obj, signed char color, bool toTheBeginning)
{
	assert((unsigned int)color < COLOR_ID_COUNT);

	typename IList<TextListData, T>::Entry entry;
	entry.name = name;
	entry.object = obj;
	entry.data.colorId = color;
  entry.data.colorBackgroundId = -1;
	entry.data.useHzAlignment = false;
	if (toTheBeginning) {
    ((IList< TextListData, T >*)this)->unshift(entry);
	} else {
    ((IList< TextListData, T >*)this)->add(entry);
	}
}

template <typename T>
void TextListComponent<T>::add(const std::string& name, const T& obj, signed char color, signed char colorBackground, HorizontalAlignment align)
{
  assert((unsigned int)color < COLOR_ID_COUNT);

  typename IList<TextListData, T>::Entry entry;
  entry.name = name;
  entry.object = obj;
  entry.data.colorId = color;
  entry.data.colorBackgroundId = colorBackground;
  entry.data.useHzAlignment = true;
  entry.data.hzAlignement = align;
  ((IList< TextListData, T >*)this)->add(entry);
}

template <typename T>
void TextListComponent<T>::changeTextAt(int index, const std::string& name)
{
  ((IList< TextListData, T >*)this)->changeCursorName(index, name);
}

template <typename T>
void TextListComponent<T>::changeBackgroundColorAt(int index, int colorIndex)
{
  ((IList< TextListData, T >*)this)->getSelectedEntryAt(index).colorBackgroundId = colorIndex;
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
void TextListComponent<T>::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	Component::applyTheme(theme, view, element, properties);

	const ThemeElement* elem = theme.getElement(view, element, "textlist");
	if (elem == nullptr)
		return;

	if (hasFlag(properties, ThemeProperties::Color))
	{
		if(elem->HasProperty("selectorColor"))
			setSelectorColor((unsigned int)elem->AsInt("selectorColor"));
		if(elem->HasProperty("selectedColor"))
			setSelectedColor((unsigned int)elem->AsInt("selectedColor"));
		if(elem->HasProperty("primaryColor"))
			setColor(0, (unsigned int)elem->AsInt("primaryColor"));
		if(elem->HasProperty("secondaryColor"))
			setColor(1, (unsigned int)elem->AsInt("secondaryColor"));
	}

	setFont(Font::getFromTheme(elem, properties, mFont));
	
	if (hasFlag(properties, ThemeProperties::Sound) && elem->HasProperty("scrollSound"))
  {
    Path soundPath = Path(elem->AsString("sound"));
    setSound(AudioManager::Instance().LoadSound(soundPath));
  }

	if (hasFlag(properties, ThemeProperties::Alignment))
	{
		if(elem->HasProperty("alignment"))
		{
			const std::string& str = elem->AsString("alignment");
			if(str == "left")
				setAlignment(HorizontalAlignment::Left);
			else if(str == "center")
				setAlignment(HorizontalAlignment::Center);
			else if(str == "right")
				setAlignment(HorizontalAlignment::Right);
			else
				LOG(LogError) << "Unknown TextListComponent alignment \"" << str << "\"!";
		}
		if(elem->HasProperty("horizontalMargin"))
		{
			mHorizontalMargin = elem->AsFloat("horizontalMargin") * (this->mParent ? this->mParent->getSize().x() : Renderer::getDisplayWidthAsFloat());
		}
	}

	if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->HasProperty("forceUppercase"))
		setUppercase(elem->AsBool("forceUppercase"));

	if (hasFlag(properties, ThemeProperties::LineSpacing))
	{
		if(elem->HasProperty("lineSpacing"))
			setLineSpacing(elem->AsFloat("lineSpacing"));
		if(elem->HasProperty("selectorHeight"))
		{
			setSelectorHeight(elem->AsFloat("selectorHeight") * Renderer::getDisplayHeightAsFloat());
		} else {
			setSelectorHeight(mFont->getSize() * 1.5);
		}
		if(elem->HasProperty("selectorOffsetY"))
		{
			float scale = this->mParent ? this->mParent->getSize().y() : Renderer::getDisplayHeightAsFloat();
			setSelectorOffsetY(elem->AsFloat("selectorOffsetY") * scale);
		} else {
			setSelectorOffsetY(0.0);
		}
	}

	if (elem->HasProperty("selectorImagePath"))
	{
		Path path(elem->AsString("selectorImagePath"));
		bool tile = elem->HasProperty("selectorImageTile") && elem->AsBool("selectorImageTile");
		mSelectorImage.setImage(path, tile);
		mSelectorImage.setSize(mSize.x(), mSelectorHeight);
		mSelectorImage.setColorShift(mSelectorColor);
	} else {
		mSelectorImage.setImage(Path());
	}
}

#include <utils/Strings.h>
#include "components/DateTimeComponent.h"
#include "Renderer.h"
#include "WindowManager.h"
#include "utils/Log.h"
#include "utils/locale/LocaleHelper.h"
#include "themes/MenuThemeData.h"

DateTimeComponent::DateTimeComponent(WindowManager&window, Display dispMode)
  : Component(window),
    mEditing(false),
    mEditIndex(0),
    mDisplayMode(dispMode),
    mRelativeUpdateAccumulator(0),
    mColor(0x777777FF),
    mOriginColor(0),
    mFont(Font::get(FONT_SIZE_SMALL, Path(FONT_PATH_LIGHT))),
    mUppercase(false),
    mAutoSize(true)
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	setFont(menuTheme->menuTextSmall.font);
	setColor(menuTheme->menuText.color);
	setOriginColor(mColor);
	mFlag = true;
	updateTextCache();
}

void DateTimeComponent::setDisplayMode(Display mode)
{
	mDisplayMode = mode;
	updateTextCache();
}

bool DateTimeComponent::ProcessInput(const InputCompactEvent& event)
{
	if (event.ValidPressed())
	{
		if(mDisplayMode != Display::RelativeToNow) //don't allow editing for relative times
			mEditing = !mEditing;

		if(mEditing)
		{
			//started editing
			mTimeBeforeEdit = mTime;
		}

		return true;
	}

	if(mEditing)
	{
		if (event.CancelPressed())
		{
			mEditing = false;
			mTime = mTimeBeforeEdit;
			updateTextCache();
			return true;
		}

		int incDir = 0;
		if (event.AnyUpPressed() || event.L1Pressed())
			incDir = 1;
		else if (event.AnyDownPressed() || event.R1Pressed())
			incDir = -1;

		if(incDir != 0)
		{
			if(mEditIndex == 0)
			{
        mTime.AddYears(incDir);
			}
			else if(mEditIndex == 1)
			{
        mTime.AddMonth(incDir);
			}
			else if(mEditIndex == 2)
			{
        mTime.AddDays(incDir);
			}

			updateTextCache();
			return true;
		}

		if (event.AnyRightPressed())
		{
			mEditIndex++;
			if(mEditIndex >= (int)mCursorBoxes.size())
				mEditIndex--;
			return true;
		}
		
		if (event.AnyLeftPressed())
		{
			mEditIndex--;
			if(mEditIndex < 0)
				mEditIndex++;
			return true;
		}
	}

	return Component::ProcessInput(event);
}

void DateTimeComponent::Update(int deltaTime)
{
	if(mDisplayMode == Display::RelativeToNow || mDisplayMode == Display::RealTime)
	{
		mRelativeUpdateAccumulator += deltaTime;
		if(mRelativeUpdateAccumulator > 1000)
		{
			mRelativeUpdateAccumulator = 0;
			updateTextCache();
		}
	}

  Component::Update(deltaTime);
}

void DateTimeComponent::Render(const Transform4x4f& parentTrans)
{
    if(mDisabled)
    {
        return;
    }

	Transform4x4f trans = parentTrans * getTransform();

	if(mTextCache)
	{
        // vertically center
        Vector3f off(0, (mSize.y() - mTextCache->metrics.size.y()) / 2, 0);
		trans.translate(off);
		trans.round();

		Renderer::SetMatrix(trans);

		std::shared_ptr<Font> font = getFont();

        mTextCache->setColor((mColor & 0xFFFFFF00) | getOpacity());
		font->renderTextCache(mTextCache.get());

		if(mEditing)
		{
			if(mEditIndex >= 0 && (unsigned int)mEditIndex < mCursorBoxes.size())
			{
				Renderer::DrawRectangle((int)mCursorBoxes[mEditIndex][0], (int)mCursorBoxes[mEditIndex][1],
					(int)mCursorBoxes[mEditIndex][2], (int)mCursorBoxes[mEditIndex][3], 0x00000022);
			}
		}
	}
}

void DateTimeComponent::setValue(const std::string& val)
{
  DateTime dt(false);
  if (DateTime::FromCompactISO6801(val, dt))
  {
    mTime = dt;
    updateTextCache();
  }
}

void DateTimeComponent::setValue(const DateTime& dt)
{
  mTime = dt;
  updateTextCache();
}

std::string DateTimeComponent::getValue() const
{
	return mTime.ToCompactISO8601();
}

DateTimeComponent::Display DateTimeComponent::getCurrentDisplayMode() const
{
	/*if(mEditing)
	{
		if(mDisplayMode == DISP_RELATIVE_TO_NOW)
		{
			//TODO: if time component == 00:00:00, return DISP_DATE, else return DISP_DATE_TIME
			return DISP_DATE;
		}
	}*/

	return mDisplayMode;
}

std::string DateTimeComponent::getDisplayString(Display mode) const
{
	char strbuf[256];

	switch(mode)
	{
	  case Display::Date: return mTime.ToStringFormat("%YYYY/%MM/%dd");
    case Display::DateTime: return mTime.ToStringFormat("%YYYY/%MM/%dd %HH:%mm:%ss");
    case Display::Time: return mTime.ToStringFormat("%HH:%mm:%ss");
    case Display::Year: return mTime.ToStringFormat("%YYYY");
    case Display::RealTime: return DateTime().ToStringFormat("%HH:%mm:%ss");
    case Display::RelativeToNow:
		{
            TimeSpan diff = DateTime() - mTime;
            if (diff.IsNegative() || diff.TotalDays() > 3560)
                return _("never");

			if (diff.TotalSeconds() < 2) return _("just now");
			if (diff.TotalSeconds() < 60) { snprintf(strbuf, sizeof(strbuf), _N("%i sec ago", "%i secs ago", diff.TotalSeconds()).c_str(), (int) diff.TotalSeconds()); return strbuf; }
			if (diff.TotalMinutes() < 60) { snprintf(strbuf, sizeof(strbuf), _N("%i min ago", "%i mins ago", diff.TotalMinutes()).c_str(), (int) diff.TotalMinutes()); return strbuf; }
			if (diff.TotalHours()   < 24) { snprintf(strbuf, sizeof(strbuf), _N("%i hour ago", "%i hours ago", diff.TotalHours()).c_str(), (int) diff.TotalHours()); return strbuf; }
			snprintf(strbuf, sizeof(strbuf), _N("%i day ago", "%i days ago", diff.TotalDays()).c_str(), (int) diff.TotalDays());
			return strbuf;
		}
	  default: break;
	}

	return "???";
}

std::shared_ptr<Font> DateTimeComponent::getFont() const
{
	if(mFont)
		return mFont;

	return Font::get(FONT_SIZE_MEDIUM);
}

void DateTimeComponent::updateTextCache()
{
	mFlag = !mFlag;
	Display mode = getCurrentDisplayMode();
	const std::string dispString = mUppercase ? Strings::ToUpperASCII(getDisplayString(mode)) : getDisplayString(mode);
	std::shared_ptr<Font> font = getFont();
	mTextCache = std::unique_ptr<TextCache>(font->buildTextCache(dispString, Vector2f(0, 0), mColor, mSize.x(), mHorizontalAlignment));

	if(mAutoSize)
	{
		mSize = mTextCache->metrics.size;

		//mAutoSize = false;
		if(getParent() != nullptr)
			getParent()->onSizeChanged();
	}

	//set up cursor positions
	mCursorBoxes.clear();

	if(dispString.empty() || mode == Display::RelativeToNow)
		return;

	//month
	Vector2f start(0, 0);
	Vector2f end = font->sizeText(dispString.substr(0, 2));
	Vector2f diff = end - start;
	mCursorBoxes.push_back(Vector4f(start[0], start[1], diff[0], diff[1]));

	//day
	start[0] = font->sizeText(dispString.substr(0, 3)).x();
	end = font->sizeText(dispString.substr(0, 5));
	diff = end - start;
	mCursorBoxes.push_back(Vector4f(start[0], start[1], diff[0], diff[1]));

	//year
	start[0] = font->sizeText(dispString.substr(0, 6)).x();
	end = font->sizeText(dispString.substr(0, 10));
	diff = end - start;
	mCursorBoxes.push_back(Vector4f(start[0], start[1], diff[0], diff[1]));

	//if mode == DISP_DATE_TIME do times too but I don't wanna do the logic for editing times because no one will ever use it so screw it
}

void DateTimeComponent::setColor(unsigned int color)
{
	mColor = color;
	if(mTextCache)
		mTextCache->setColor(color);
}

void DateTimeComponent::setFont(const std::shared_ptr<Font>& font)
{
	mFont = font;
	updateTextCache();
}

void DateTimeComponent::setHorizontalAlignment(TextAlignment align)
{
	mHorizontalAlignment = align;
	updateTextCache();
}

void DateTimeComponent::onSizeChanged()
{
	mAutoSize = false;
	updateTextCache();
}

void DateTimeComponent::setUppercase(bool uppercase)
{
	mUppercase = uppercase;
	updateTextCache();
}

void DateTimeComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
	const ThemeElement* elem = theme.getElement(view, element, "datetime");
	if(elem == nullptr)
		return;

	// We set mAutoSize BEFORE calling GuiComponent::applyTheme because it calls
	// setSize(), which will call updateTextCache(), which will reset mSize if 
	// mAutoSize == true, ignoring the theme's value.
	if (hasFlag(properties, ThemeProperties::Size))
		mAutoSize = !elem->HasProperty("size");

	Component::applyTheme(theme, view, element, properties);

	if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("color"))
		setColor((unsigned int)elem->AsInt("color"));

	if (hasFlag(properties, ThemeProperties::Display) && elem->HasProperty("display"))
	{
        std::string str = elem->AsString("display");
        if(str == "date")
        setDisplayMode(Display::Date);
        if(str == "dateTime")
            setDisplayMode(Display::DateTime);
        else if(str == "year")
            setDisplayMode(Display::Year);
        else if(str == "realTime")
            setDisplayMode(Display::RealTime);
        else if(str == "time")
            setDisplayMode(Display::Time);
        else if(str == "RelativeToNow")
            setDisplayMode(Display::RelativeToNow);
        else
        { LOG(LogError) << "[DateTimeComponent] Unknown date time display mode string: " << str; }
  }

	if (hasFlag(properties, ThemeProperties::Alignment) && elem->HasProperty("alignment"))
	{
		std::string str = elem->AsString("alignment");
		if(str == "left")
			setHorizontalAlignment(TextAlignment::Left);
		else if(str == "center")
			setHorizontalAlignment(TextAlignment::Center);
		else if(str == "right")
			setHorizontalAlignment(TextAlignment::Right);
		else
    { LOG(LogError) << "[DateTimeComponent] Unknown text alignment string: " << str; }
	}

	if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->HasProperty("forceUppercase"))
		setUppercase(elem->AsBool("forceUppercase"));

	setFont(Font::getFromTheme(elem, properties, mFont));
}

//
// Created by bkg2k on 21/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <components/EditableComponent.h>
#include <guis/GuiArcadeVirtualKeyboard.h>
#include <utils/Strings.h>
#include <utils/Log.h>
#include <themes/ThemeData.h>
#include <Renderer.h>
#include <WindowManager.h>

EditableComponent::EditableComponent(WindowManager& window)
  : Component(window)
  , mBackground(window, Path(":/textinput_ninepatch.png"))
  , mFont(Font::get(FONT_SIZE_MEDIUM))
  , mInterface(nullptr)
  , mIndentifier(0)
  , mColor(0x000000FF)
  , mOriginColor(0x000000FF)
  , mLineSpacing(1.5f)
  , mHorizontalAlignment(TextAlignment::Left)
  , mVerticalAlignment(TextAlignment::Center)
  , mColorOpacity(0xFF)
  , mUppercase(false)
  , mAutoCalcExtentX(true)
  , mAutoCalcExtentY(true)
  , mMasked(false)
{
  mPosition = Vector3f::Zero();
  mSize = Vector2f::Zero();
  mBackground.setCenterColor(0xFFFFFF80/*0x00000020*/);
  mBackground.setEdgeColor(0xFFFFFF80/*0x00000020*/);
}

EditableComponent::EditableComponent(WindowManager&window, const std::string& editTitle, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, int id, IEditableComponent* interface, bool masked)
  : EditableComponent(window)
{
  mMasked = masked;
  mIndentifier = id;
  mInterface = interface;
  mFont = font;
  mText = text;
  mEditTitle = editTitle;
  mColor = color;
  mColorOpacity = (unsigned char)(color & 0xFF);
  mOriginColor = color;
  onTextChanged();
}

EditableComponent::EditableComponent(WindowManager&window, const std::string& editTitle, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, const std::function<void(const std::string&)>& callback)
  : EditableComponent(window)
{
  mTextChanged = callback;
  mFont = font;
  mText = text;
  mEditTitle = editTitle;
  mColor = color;
  mColorOpacity = (unsigned char)(color & 0xFF);
  mOriginColor = color;
  onTextChanged();
}

EditableComponent::EditableComponent(WindowManager&window, const std::string& editTitle, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align, const std::function<void(const std::string&)>& callback)
  : EditableComponent(window)
{
  mTextChanged = callback;
  mFont = font;
  mText = text;
  mEditTitle = editTitle;
  mColor = color;
  mColorOpacity = (unsigned char)(color & 0xFF);
  mOriginColor = color;
  mHorizontalAlignment = align;
  onTextChanged();
}

void EditableComponent::onSizeChanged()
{
  mAutoCalcExtentX = (getSize().x() == 0);
  mAutoCalcExtentY = (getSize().y() == 0);
  onTextChanged();
}

void EditableComponent::setFont(const std::shared_ptr<Font>& font)
{
  mFont = font;
  onTextChanged();
}

void EditableComponent::setColor(unsigned int color)
{
  mColor = color;
  unsigned char opacity = mColor & 0x000000FF;
  Component::setOpacity(opacity);

  mColorOpacity = mColor & 0x000000FF;

  onColorChanged();
}

//  Scale the opacity
void EditableComponent::setOpacity(unsigned char opacity)
{
  // This method is mostly called to do fading in-out of the Text component element.
  // Therefore, we assume here that opacity is a fractional value (expressed as an int 0-255),
  // of the opacity originally set with setColor() or setBackgroundColor().

  unsigned char o = (unsigned char)((float)opacity / 255.f * (float) mColorOpacity);
  mColor = (mColor & 0xFFFFFF00) | (unsigned char) o;

  onColorChanged();

  Component::setOpacity(opacity);
}

void EditableComponent::setText(const std::string& text)
{
  mText = mUppercase ? Strings::ToUpperUTF8(text) : text;
  onTextChanged();
}

void EditableComponent::setUppercase(bool uppercase)
{
  mUppercase = uppercase;
  onTextChanged();
}

void EditableComponent::Render(const Transform4x4f& parentTrans)
{
  if(mThemeDisabled) return;
  Transform4x4f trans = parentTrans * getTransform();

  Renderer::SetMatrix(trans);
  if (mBackground.getSize().x() != mSize.x() &&
      mBackground.getSize().y() != mSize.y())
    mBackground.setSize(mSize);
  mBackground.Render(trans);

  if(mTextCache)
  {
    const Vector2f& textSize = mTextCache->metrics.size;
    float yOff = 0;
    switch(mVerticalAlignment)
    {
      case TextAlignment::Top: yOff = 0; break;
      case TextAlignment::Bottom: yOff = (getSize().y() - textSize.y()); break;
      case TextAlignment::Center: yOff = (getSize().y() - textSize.y()) / 2.0f; break;
      case TextAlignment::Left:
      case TextAlignment::Right: break;
    }
    Vector3f off(mBackground.MargingX(), yOff, 0);

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
      float usableSize = mSize.x() - mBackground.MargingX() * 2;
      switch(mHorizontalAlignment)
      {
        case TextAlignment::Left:
          Renderer::DrawRectangle(mBackground.MargingX(), 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
          break;
        case TextAlignment::Center:
          Renderer::DrawRectangle(mBackground.MargingX() +(usableSize - mTextCache->metrics.size.x()) / 2.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
          break;
        case TextAlignment::Right:
          Renderer::DrawRectangle(mBackground.MargingX() + usableSize - mTextCache->metrics.size.x(), 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
          break;
        case TextAlignment::Top:
        case TextAlignment::Bottom:break;
      }
    }

    mFont->renderTextCache(mTextCache.get());
  }

}

void EditableComponent::calculateExtent()
{
  if(mAutoCalcExtentX)
  {
    mSize = mFont->sizeText(mText, mLineSpacing) + mBackground.MargingX() * 2;
  }else{
    if(mAutoCalcExtentY)
    {
      mSize[1] = mFont->sizeWrappedText(mText, getSize().x(), mLineSpacing).y();
    }
  }
}

void EditableComponent::onTextChanged()
{
  calculateExtent();

  if(!mFont || mText.empty())
  {
    mTextCache.reset();
    return;
  }

  std::string text = mMasked ? std::string(mText.size(), '*') : (mUppercase ? Strings::ToUpperUTF8(mText) : mText);

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
  float usableSize = mSize.x() - mBackground.MargingX() * 2;
  if(!isMultiline && (mSize.x() != 0) && !text.empty() && (size.x() > usableSize || addAbbrev))
  {
    // abbreviate text
    const std::string abbrev = "\u2026";
    Vector2f abbrevSize = f->sizeText(abbrev);

    while(!text.empty() && size.x() + abbrevSize.x() > usableSize)
    {
      size_t newSize = Font::getPrevCursor(text, text.size());
      text.erase(newSize, text.size() - newSize);
      size = f->sizeText(text);
    }

    text.append(abbrev);

    mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(text, Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, usableSize, mHorizontalAlignment, mLineSpacing));
  }else{
    mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(f->wrapText(text, mSize.x()), Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, usableSize, mHorizontalAlignment, mLineSpacing));
  }
}

void EditableComponent::onColorChanged()
{
  if(mTextCache)
  {
    mTextCache->setColor(mColor);
  }
}

void EditableComponent::setHorizontalAlignment(TextAlignment align)
{
  mHorizontalAlignment = align;
  onTextChanged();
}

void EditableComponent::setLineSpacing(float spacing)
{
  mLineSpacing = spacing;
  onTextChanged();
}

void EditableComponent::applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties)
{
  Component::applyTheme(theme, view, element, properties);

  const ThemeElement* elem = theme.getElement(view, element, "text");
  if(elem == nullptr) return;

  if (hasFlag(properties, ThemeProperties::Color) && elem->HasProperty("color"))
    setColor((unsigned int)elem->AsInt("color"));

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
    { LOG(LogError) << "[EditableComponent] Unknown text alignment string: " << str; }
  }

  if (hasFlag(properties, ThemeProperties::Text) && elem->HasProperty("text"))
    setText(elem->AsString("text"));

  if (hasFlag(properties, ThemeProperties::ForceUppercase) && elem->HasProperty("forceUppercase"))
    setUppercase(elem->AsBool("forceUppercase"));

  if (hasFlag(properties, ThemeProperties::LineSpacing) && elem->HasProperty("lineSpacing"))
    setLineSpacing(elem->AsFloat("lineSpacing"));

  setFont(Font::getFromTheme(elem, properties, mFont));
}

void EditableComponent::StartEditing()
{
  mTextBackup = mText;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, mEditTitle, mTextBackup, this));
}

void EditableComponent::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  setText(text);
}

void EditableComponent::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  (void)text;
  if (mTextChanged)
    mTextChanged(text);
  if (mInterface != nullptr)
  {
    mInterface->EditableComponentTextChanged(mIndentifier, text);
    mParent->onSizeChanged();
  }
}

void EditableComponent::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk)
{
  (void)vk;
  setText(mTextBackup);
}
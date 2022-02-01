//
// Created by bkg2k on 11/10/2019.
//

#include <Renderer.h>
#include <utils/Strings.h>
#include <input/InputCompactEvent.h>
#include <help/Help.h>
#include <utils/locale/LocaleHelper.h>
#include "GuiArcadeVirtualKeyboard.h"
#include "utils/Log.h"

const wchar_t* const GuiArcadeVirtualKeyboard::sWheels[sWheelCount] =
{
  GuiArcadeVirtualKeyboard::sWheelAlphaSmalls   ,
  GuiArcadeVirtualKeyboard::sWheelAlphaCaps     ,
  GuiArcadeVirtualKeyboard::sWheelAlphaDigits   ,
  GuiArcadeVirtualKeyboard::sWheelAlphaSymbols  ,
  GuiArcadeVirtualKeyboard::sWheelDiacriticSmall,
  GuiArcadeVirtualKeyboard::sWheelDiacriticCaps ,
};

GuiArcadeVirtualKeyboard::GuiArcadeVirtualKeyboard(WindowManager& window, const std::string& title, const std::string& initValue,
                                                   IGuiArcadeVirtualKeyboardInterface* okCallback)
  : Gui(window),
    mSavedAngles(),
    mTitle(title),
    mCurrentWheel(),
    mPreviousWheel(),
    mWheelDimmingColor(0),
    mWheelDimming(false),
    mShown(false),
    mCursor(0),
    mSelectionLength(0),
    mOffsetInPixel(0),
    mCursorOn(false),
    mCursorFast(false),
    mCursorDirection(Direction::None),
    mCursorTimerMs(0),
    mWheelChangeAnimation(0),
    mCharChangeAnimation(0),
    mOuterEditor(),
    mInnerEditor(),
    mMoveDirection(Direction::None),
    mMoveFast(false),
    mMoveOn(false),
    mValidateCallback(okCallback),
    mWheelFont(Font::get(GetFontSize(sWheelFontRatio), Path(":/vk.ttf"))),
    mTitleFont(Font::get(GetFontSize(sTitleFontRatio))),
    mTextFont(Font::get(GetFontSize(sTextFontRatio))),
    mSelectedChar(TextureResource::get(Path(":/plaincircle30.png"), false, true, true))
{
  ChangeWheel(0);

  // Compute editor bounds
  double ray = Renderer::Instance().DisplayWidthAsFloat();
  ray = ((ray / 2.0) - (float)GetFontSize(sWheelFontRatio)) - (ray / 20.0f);
  float centerX = Renderer::Instance().DisplayWidthAsFloat() / 2.0f;
  float centerY = Renderer::Instance().DisplayHeightAsFloat() / 2.0f;

  float fontHeight  = (float)mTextFont->getMaxHeight();

  mInnerEditor.x = centerX - (float)ray;
  mInnerEditor.y = centerY - (fontHeight / 2.0f);
  mInnerEditor.w = (float)(ray * 2.0);
  mInnerEditor.h = fontHeight;

  mOuterEditor.x = mInnerEditor.x - (fontHeight / 4.0f);
  mOuterEditor.y = mInnerEditor.y - (fontHeight / 4.0f);
  mOuterEditor.w = mInnerEditor.w + (fontHeight / 2.0f);
  mOuterEditor.h = mInnerEditor.h + (fontHeight / 2.0f);

  // Set cursor
  mText = Strings::Utf8ToUnicode(initValue);
  mCursor = mText.size();
  AdjustCursor();

  // Prepare title text
  PrepareTitle();

  SDL_StartTextInput();
}

void GuiArcadeVirtualKeyboard::ChangeChar(bool fast, bool left, bool on)
{
  if (on) StartMoving(left, fast);
  else StopMoving();
}

void GuiArcadeVirtualKeyboard::StartMoving(bool left, bool fast)
{
  mMoveDirection = left ? Direction::Left : Direction::Right;
  mMoveFast = fast;
  mMoveOn = true;
}

void GuiArcadeVirtualKeyboard::StopMoving()
{
  mMoveOn = false;
}

void GuiArcadeVirtualKeyboard::ChangeWheel(int delta)
{
  // We're still in a previous change
  if (mWheelChangeAnimation != 0) return;

  // Kill any char animation
  mCharChangeAnimation = 0;

  // Save previous wheel & angle
  mPreviousWheel = mCurrentWheel;
  mSavedAngles[mCurrentWheel.mIndex] = mCurrentWheel.mAngle;
  // Move
  int newIndex = mCurrentWheel.mIndex + delta;

  // Build new wheel
  BuildWheel(mCurrentWheel, newIndex);
}

void GuiArcadeVirtualKeyboard::BuildWheel(Wheel& wheel, int index)
{
  // Control index
  if (index < 0) index = sWheelCount - 1;
  if (index >= sWheelCount) index = 0;

  // Store index
  wheel.mIndex = index;

  // Build new array
  wheel.mWheelCharCount = wcslen(sWheels[index]);
  double charCountDouble = (double)wheel.mWheelCharCount;

  // Get current character and adjust current angle
  wheel.mAngle = mSavedAngles[index];
  int charIndex = GetCurrentCharIndex(wheel);
  wheel.mAngle = ((2.0 * Pi) * (double)charIndex) / charCountDouble;

  // Start wheel change animation
  mWheelChangeAnimation = sWheelChangeAnimationTimeMs;
}

int GuiArcadeVirtualKeyboard::GetCurrentCharIndex(const Wheel& wheel)
{
  // Get section size
  double section = (2.0 * Pi) / (double)wheel.mWheelCharCount;

  // Get position in section
  double position = ((2.0 * Pi) - wheel.mAngle) / section;
  // Integer part is the low index
  int index = (int)position;
  double fpart = position - (double)index;

  // If fractional part > 0.5, current angle is close to next section
  if (fpart >= 0.5) index++;
  // Ensure we don't get beyond the wheel
  index %= wheel.mWheelCharCount;

  return index;
}

void GuiArcadeVirtualKeyboard::textInput(const char* text)
{
  //{ LOG(LogDebug) << "[ArcacdeVirtualKeyboard] TextInput: " << text << " (" << strlen(text) << ")"; }
  std::vector<unsigned int> unicodes = Strings::Utf8ToUnicode(text);
  for(unsigned int unicode : unicodes)
    AddCharacter(unicode);
  AdjustCursor();
}

bool GuiArcadeVirtualKeyboard::ProcessInput(const InputCompactEvent& event)
{
  // Switch between keyboard & pads
  mWheelDimming = event.IsKeyboard();

  if (event.IsKeyboard())
  {
    //{ LOG(LogDebug) << "[ArcadeVirtualKeyboard] Type: " << (int)event.RawEvent().Type() << " - Id: " << event.RawEvent().Id() << " - Device: " << event.RawEvent().Device() << " - Value: " << event.RawEvent().Value(); }
    bool pressed = event.RawEvent().Value() != 0;
    switch(event.RawEvent().Id())
    {
      case SDLK_BACKSPACE: if (pressed) Backspace(); break;
      case SDLK_DELETE: if (pressed) Delete(); break;
      case SDLK_RETURN:
      case SDLK_RETURN2: if (pressed) ValidateEditor(); break;
      case SDLK_ESCAPE: if (pressed) CancelEditor(); break;
      case SDLK_LEFT: ChangeCursor(true, false, pressed); break;
      case SDLK_RIGHT: ChangeCursor(false, false, pressed); break;
    }
  }
  else
  {
    bool updown = event.UpPressed() || event.DownPressed();
    // Change Char (must be tested BEFORE any other action, otherwise Circle won't stop rotating)
    if (event.Left() && !event.RightPressed() && !updown) ChangeChar(false, false, event.LeftPressed());
    else if (event.Right() && !event.LeftPressed() && !updown) ChangeChar(false, true, event.RightPressed());
    else if (event.L2()) ChangeChar(true, false, event.L2Pressed());
    else if (event.R2()) ChangeChar(true, true, event.R2Pressed());
    else if (event.J1Left()) ChangeChar(true, false, event.J1LeftPressed());
    else if (event.J1Right()) ChangeChar(true, true, event.J1RightPressed());
    // Change Circles
    else if (event.UpPressed()) ChangeWheel(-1);
    else if (event.DownPressed()) ChangeWheel(1);
    // Move cursor
    else if (event.L1()) ChangeCursor(true, false, event.L1Pressed());
    else if (event.R1()) ChangeCursor(false, false, event.R1Pressed());
    else if (event.J2Left()) ChangeCursor(true, true, event.J2LeftPressed());
    else if (event.J2Right()) ChangeCursor(false, true, event.J2RightPressed());
    // Edit
    else if (event.ValidPressed()) AddCharacter(GetSelectedCharacter());
    else if (event.CancelPressed()) CancelEditor();
    else if (event.StartPressed()) ValidateEditor();
    else if (event.YPressed()) Backspace();
    else if (event.XPressed()) Delete();
  }

  // VK consume everything
  return true;
}

void GuiArcadeVirtualKeyboard::ResetCursorBlink()
{
  mCursorBlinkReference = DateTime();
}

unsigned int GuiArcadeVirtualKeyboard::GetSelectedCharacter()
{
  int index = GetCurrentCharIndex(mCurrentWheel);
  return sWheels[mCurrentWheel.mIndex][index];
}

void GuiArcadeVirtualKeyboard::AddCharacter(unsigned int unicode)
{
  if (mCursor < (int)mText.size())
    mText.insert(mText.begin() + mCursor, unicode);
  else
    mText.push_back(unicode);

  mCursor++;
  AdjustCursor();
  ResetCursorBlink();

  if (mValidateCallback != nullptr)
    mValidateCallback->ArcadeVirtualKeyboardTextChange(*this, Strings::UnicodeToUtf8(mText));
}

void GuiArcadeVirtualKeyboard::CancelEditor()
{
  if (mValidateCallback != nullptr)
    mValidateCallback->ArcadeVirtualKeyboardCanceled(*this);

  Close();

  if (mValidateCallback != nullptr)
    mValidateCallback->ArcadeVirtualKeyboardHide(*this);
}

void GuiArcadeVirtualKeyboard::ValidateEditor()
{
  if (mValidateCallback != nullptr)
    mValidateCallback->ArcadeVirtualKeyboardValidated(*this, Strings::UnicodeToUtf8(mText));

  Close();

  if (mValidateCallback != nullptr)
    mValidateCallback->ArcadeVirtualKeyboardHide(*this);
}

void GuiArcadeVirtualKeyboard::Delete()
{
  if (mCursor < (int)mText.size())
  {
    mText.erase(mText.begin() + mCursor);
    AdjustCursor((int)mInnerEditor.w / 4);
    ResetCursorBlink();

    if (mValidateCallback != nullptr)
      mValidateCallback->ArcadeVirtualKeyboardTextChange(*this, Strings::UnicodeToUtf8(mText));
  }
}

void GuiArcadeVirtualKeyboard::Backspace()
{
  if (mCursor > 0)
  {
    mCursor--;
    mText.erase(mText.begin() + mCursor);
    AdjustCursor(-((int)mInnerEditor.w / 4));
    ResetCursorBlink();

    if (mValidateCallback != nullptr)
      mValidateCallback->ArcadeVirtualKeyboardTextChange(*this, Strings::UnicodeToUtf8(mText));
  }
}

void GuiArcadeVirtualKeyboard::Update(int deltaTime)
{
  // Rotation
  switch(mMoveDirection)
  {
    case Direction::Left:
    case Direction::Right:
    {
      bool left = mMoveDirection == Direction::Left;
      double direction = left ? -1.0 : 1.0;
      double section = (2.0 * Pi) / (double)mCurrentWheel.mWheelCharCount;
      if (mMoveOn)
      {
        // Move in the right direction at the right speed, continuously
        double diviser = mMoveFast ? sWheelRotateAnimationFastTimeMs : sWheelRotateAnimationSlowTimeMs;
        mCurrentWheel.mAngle += direction * section * (double)deltaTime / diviser;
      }
      else
      {
        // Get target angle
        double floatChar = mCurrentWheel.mAngle / section;
        double targetAngle = left ? (int)floatChar * section : ((int)floatChar + 1) * section;
        // If the speed is too low, stop the animation
        double delta = targetAngle - mCurrentWheel.mAngle;
        if (delta < 0) delta = -delta;
        if (delta < (section / 50))
        {
          mMoveDirection = Direction::None;
          mCurrentWheel.mAngle = targetAngle;
        }
        else
          mCurrentWheel.mAngle += (targetAngle - mCurrentWheel.mAngle) * 0.3;
      }
      break;
    }
    case Direction::None:
    default: break;
  }
  while(mCurrentWheel.mAngle > (2.0 * Pi)) mCurrentWheel.mAngle -= (2.0 * Pi);
  while(mCurrentWheel.mAngle < 0.0) mCurrentWheel.mAngle += (2.0 * Pi);

  // Run Wheel animation
  if ((mWheelChangeAnimation -= deltaTime) < 0)
    mWheelChangeAnimation = 0;

  // Cursor
  MoveEditCursor(deltaTime);

  // Wheel dimming
  if (mWheelDimmingColor < sWheelDimmingTransparency && mWheelDimming)
    if ((mWheelDimmingColor += deltaTime) > sWheelDimmingTransparency)
      mWheelDimmingColor = sWheelDimmingTransparency;
  if (mWheelDimmingColor > 0 && !mWheelDimming)
    if ((mWheelDimmingColor -= deltaTime) < 0)
      mWheelDimmingColor = 0;
}

void GuiArcadeVirtualKeyboard::RenderEditedString()
{
  Font::Glyph& findTheBaseline = mTextFont->Character('g');
  float baseline = (findTheBaseline.texSize.y() * (float)findTheBaseline.texture->textureSize.y()) - findTheBaseline.bearing.y();

  float offsetX = mInnerEditor.x - mOffsetInPixel;
  float offsetY = mInnerEditor.y + mInnerEditor.h - baseline;

  int cursorLight = (int)(DateTime() - mCursorBlinkReference).TotalMilliseconds() % 400 - 200;
  if (cursorLight < 0) cursorLight = -cursorLight;
  unsigned int cursorColor = (cursorLight << 24) | (cursorLight << 16) | (cursorLight << 8) | 0xFF;

  for(int i = 0; i < (int)mText.size(); i++)
  {
    unsigned int unicode = mText[i];
    const Font::Glyph& glyph = mTextFont->Character(unicode);
    float width = glyph.advance.x();

    // Inside selection?
    if (mCursor <= i && i < mCursor + mSelectionLength)
      Renderer::DrawRectangle(offsetX, mInnerEditor.y, width, mInnerEditor.h, 0xFFFFFFA0);
      //Renderer::drawRect(mOuterEditor.x, mOuterEditor.y, mOuterEditor.w, mOuterEditor.h, 0x00000080);

    // Cursor itself
    if (i == mCursor)
      Renderer::DrawRectangle(offsetX, mInnerEditor.y, width, mInnerEditor.h, cursorColor);

    mTextFont->renderCharacter(unicode, offsetX + glyph.bearing.x(), offsetY - glyph.bearing.y(), 1.0f, 1.0f, sEditableTextColor);
    offsetX += width;
  }
  if (mCursor >= (int)mText.size())
  {
    Renderer::DrawRectangle(offsetX, mInnerEditor.y, mTextFont->Character(32).advance.x(), mInnerEditor.h, cursorColor);
  }
}

void GuiArcadeVirtualKeyboard::ChangeCursor(bool left, bool fast, bool on)
{
  if (on) StartMovingCursor(left, fast);
  else StopMovingCursor();
}

void GuiArcadeVirtualKeyboard::StartMovingCursor(bool left, bool fast)
{
  mCursorDirection = left ? Direction::Left : Direction::Right;
  mCursorFast = fast;
  mCursorOn = true;
  // First move occurs immediately
  MoveEditCursor(0);
  mCursorTimerMs = mCursorFast ? sMoveCursorAnimationSlowTimeMs : sMoveCursorAnimationFirstTimeMs;
}

void GuiArcadeVirtualKeyboard::StopMovingCursor()
{
  mCursorOn = false;
}

void GuiArcadeVirtualKeyboard::MoveEditCursor(int deltatime)
{
  // Cursor moving?
  if (!mCursorOn) return;

  // Check time
  if (deltatime != 0)
  {
    // Check timer
    if ((mCursorTimerMs -= deltatime) > 0) return;
    // Reset timer
    mCursorTimerMs = mCursorFast ? sMoveCursorAnimationFastTimeMs : sMoveCursorAnimationSlowTimeMs;
  }

  // Adjust cursor
  mCursor += mCursorDirection == Direction::Left ? -1 : 1;
  if (mCursor < 0) mCursor = 0;
  if (mCursor > (int)mText.size()) mCursor = mText.size();
  AdjustCursor();
  ResetCursorBlink();
}

void GuiArcadeVirtualKeyboard::AdjustCursor(int cursorextend)
{
  // Get cursor size - Use space char as template for cursor at end of string
  unsigned int unicode = (mCursor < (int)mText.size()) ? mText[mCursor] : 32;
  float cursorWidth = mTextFont->Character(unicode).advance.x();

  // Get current cursor position
  float cursorLeft = 0;
  for(int i = 0; i < mCursor; i++)
    cursorLeft += mTextFont->Character(mText[i]).advance.x();
  float cursorRight = cursorLeft + cursorWidth;

  // Add extension
  if (cursorextend < 0) cursorLeft += (float)cursorextend;
  if (cursorextend > 0) cursorRight += (float)cursorextend;

  // Adjust offset
  if (cursorRight > mOffsetInPixel + mInnerEditor.w)
    mOffsetInPixel = cursorRight - mInnerEditor.w;
  else if (cursorLeft < mOffsetInPixel)
    mOffsetInPixel = cursorLeft;
  if (mOffsetInPixel < 0)
    mOffsetInPixel = 0;
}

void GuiArcadeVirtualKeyboard::RenderSelector()
{
  struct Vertex
  {
    Vector2f pos;
    Vector2f tex;
  } vertices[6];

  double yray = Renderer::Instance().DisplayHeightAsFloat();
  yray = ((yray - (float)GetFontSize(sWheelFontRatio)) / 2.0f) - (yray / 60.0);
  yray = ((Renderer::Instance().DisplayHeightAsFloat() / 2.0) - yray);

  float x1 = (float)((Renderer::Instance().DisplayWidthAsFloat() / 2.0) - (yray * 1.5f));
  float y1 = -(float)yray * 0.5f;
  float x2 = x1 + (float)yray * 3.0f;
  float y2 = (float)yray * 2.5f;

  vertices[0].pos.Set(x1, y1);
  vertices[1].pos.Set(x1, y2);
  vertices[2].pos.Set(x2, y1);

  vertices[3].pos.Set(x2, y1);
  vertices[4].pos.Set(x1, y2);
  vertices[5].pos.Set(x2, y2);

  float tx = 0.0f;
  float ty = 0.0f;
  float sx = 1.0;
  float sy = 1.0;

  vertices[0].tex.Set(tx, ty);
  vertices[1].tex.Set(tx, sy);
  vertices[2].tex.Set(sx, ty);

  vertices[3].tex.Set(sx, ty);
  vertices[4].tex.Set(tx, sy);
  vertices[5].tex.Set(sx, sy);

  GLubyte colors[6*4];
  Renderer::BuildGLColorArray(colors, 0xFFFFFFFF, 6);

  mSelectedChar->bind();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].pos);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].tex);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

unsigned int GuiArcadeVirtualKeyboard::BlendColor(unsigned int from, unsigned int to, double ratio)
{
  int r = (int)(ratio * 255);
  int rs = (int)(from >> 24);
  int gs = (int)((from >> 16) & 0xFF);
  int bs = (int)((from >>  8) & 0xFF);
  int as = (int)((from >>  0) & 0xFF);
  int rd = (int)(to >> 24);
  int gd = (int)((to >> 16) & 0xFF);
  int bd = (int)((to >>  8) & 0xFF);
  int ad = (int)((to >>  0) & 0xFF);
  int rr = rs + (((rd - rs) * r) >> 8);
  int gr = gs + (((gd - gs) * r) >> 8);
  int br = bs + (((bd - bs) * r) >> 8);
  int ar = as + (((ad - as) * r) >> 8);
  return ((unsigned int)rr << 24) | ((unsigned int)gr << 16) | ((unsigned int)br << 8) | (unsigned int)ar;
}

void GuiArcadeVirtualKeyboard::RenderWheelChar(const Wheel& wheel, double centerx, double centery, int charindex, double xray, double yray, double sizeratio, double morphangle)
{
  // Get the real angle and adjust in [0..2*Pi]
  double angle = (((2.0 * Pi) * (double)charindex) / (double)wheel.mWheelCharCount) + wheel.mAngle;
  while(angle > (2.0 * Pi)) angle -= (2.0 * Pi);
  while(angle < 0.0) angle += (2.0 * Pi);

  // Real coordinates relative to wheel center
  float x = (float)(cos(angle - (Pi / 2.0)) * xray + centerx);
  float y = (float)(sin(angle - (Pi / 2.0)) * yray + centery);

  // Get the ratio of the "selected char" and adjust size and color
  double rawRatio = 0.0;

  if ((angle < morphangle) || (angle > (2.0 * Pi) - morphangle))
  {
    double ang = (angle < morphangle ? angle : (2.0 * Pi) - angle);
    rawRatio = (morphangle - ang) / morphangle;
  }

  float sizeRatio = 1.0f + (0.5f * (float)rawRatio);
  unsigned int alpha = 255 - (unsigned int)mWheelDimmingColor;
  unsigned int color = BlendColor(sWheelCharColorNormal, sWheelCharColorSelected, rawRatio);
  color = ((color >> 8) << 8) | alpha;

  float wr = 1.0f * sizeRatio * (float)(sizeratio * sizeratio);
  float hr = 1.0f * sizeRatio * (float)(sizeratio * sizeratio);

  unsigned int unicode = (unsigned char)sWheels[wheel.mIndex][charindex];
  Font::Glyph& glyph = mWheelFont->Character(unicode);

  float glyphWidth = glyph.texSize.x() * (float)glyph.texture->textureSize.x();
  float glyphHeight = glyph.texSize.y() * (float)glyph.texture->textureSize.y();

  x -= glyphWidth * wr / 2.0f;
  y -= glyphHeight * hr / 2.0f;

  mWheelFont->renderCharacter(unicode, x - 2, y - 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x + 0, y - 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x + 2, y - 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x + 2, y + 0, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x + 2, y + 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x + 0, y + 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x - 2, y + 2, wr, hr, alpha);
  mWheelFont->renderCharacter(unicode, x - 2, y + 0, wr, hr, alpha);

  mWheelFont->renderCharacter(unicode, x, y, wr, hr, color);
}

void GuiArcadeVirtualKeyboard::RenderWheel(const Wheel& wheel, double raymultiplier)
{
  // Get some values
  double charCountDouble = (double)wheel.mWheelCharCount;
  double section = (2.0 * Pi) / charCountDouble;
  double morphAngle = 2.0 * section;

  // Get screen center
  float centerX = Renderer::Instance().DisplayWidthAsFloat() / 2.0f;
  float centerY = Renderer::Instance().DisplayHeightAsFloat() / 2.0f;
  //Renderer::drawRect(centerX - 1.0f, 0.0f, 3.0f, Renderer::Instance().DisplayHeightAsFloat(), 0xFFFFFFFF);

  int selectedChar = GetCurrentCharIndex(wheel);
  //{ LOG(LogDebug) << "[ArcadeVirtualKeyboard] Selected: " << selectedChar << '/' << wheel.mWheelCharCount; }
  for(int i=wheel.mWheelCharCount; --i >= 0;)
    if (i != selectedChar)
    {
      double raymc = ((i & 1) != 0) ? raymultiplier : 1.0 /raymultiplier;
      // Compute the wheel size
      double xray = Renderer::Instance().DisplayWidthAsFloat();
      xray = (((xray - (float)GetFontSize(sWheelFontRatio)) / 2.0f) - (xray / 60.0)) * raymc;
      double yray = Renderer::Instance().DisplayHeightAsFloat();
      yray = (((yray - (float)GetFontSize(sWheelFontRatio)) / 2.0f) - (yray / 60.0)) * raymc;

      RenderWheelChar(wheel, centerX, centerY, i, xray, yray, raymc, morphAngle);
    }

  double raymc = ((selectedChar & 1) != 0) ? raymultiplier : 1.0 /raymultiplier;
  // Compute the wheel size
  double xray = Renderer::Instance().DisplayWidthAsFloat();
  xray = (((xray - (float)GetFontSize(sWheelFontRatio)) / 2.0f) - (xray / 60.0)) * raymc;
  double yray = Renderer::Instance().DisplayHeightAsFloat();
  yray = (((yray - (float)GetFontSize(sWheelFontRatio)) / 2.0f) - (yray / 60.0)) * raymc;

  RenderWheelChar(wheel, centerX, centerY, selectedChar, xray, yray, raymc, morphAngle);
}

void GuiArcadeVirtualKeyboard::PrepareTitle()
{
  TextCache* cache = mTitleFont->buildTextCache(mTitle,
                                                mInnerEditor.x,
                                                mOuterEditor.y - mTitleFont->getMaxHeight() * 1.2f,
                                                sTitleTextColor, true);
  mTitleCache = std::unique_ptr<TextCache>(cache);
}

void GuiArcadeVirtualKeyboard::RenderTextBox()
{
  Renderer::DrawRectangle(mOuterEditor.x, mOuterEditor.y, mOuterEditor.w, mOuterEditor.h, 0x00000080);

  // Render title
  mTitleFont->renderTextCache(mTitleCache.get());

  // Clip text display
  Renderer::Instance().PushClippingRect(Vector2i((int)mInnerEditor.x, (int)mInnerEditor.y), Vector2i((int)mInnerEditor.w, (int)mInnerEditor.h));
  // Render text
  RenderEditedString();
  // Pop clip
  Renderer::Instance().PopClippingRect();
}

void GuiArcadeVirtualKeyboard::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(0.f, 0.f, Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat(), 0x00000040);

  // Render selector
  RenderSelector();

  // Render wheels
  if (mWheelChangeAnimation > 0)
  {
    // Compute animations
    double current = (double)mWheelChangeAnimation / sWheelChangeAnimationTimeMs;
    double previous  = 1.0 - current;
    current = ((current * 10.0) * (current * 10.0)) / 100.0;
    previous = ((previous * 10.0) * (previous * 10.0)) / 100.0;

    if (mPreviousWheel.mWheelCharCount != 0) // initialized?
      RenderWheel(mPreviousWheel, 1.0 + previous);
    RenderWheel(mCurrentWheel, 1.0 + current);
  }
  else RenderWheel(mCurrentWheel, 0.98);

  // Render textbox
  RenderTextBox();

  // Callback
  if (!mShown)
  {
    if (mValidateCallback != nullptr)
      mValidateCallback->ArcadeVirtualKeyboardShow(*this);
    mShown = true;
  }
}

bool GuiArcadeVirtualKeyboard::getHelpPrompts(Help& help)
{
  help.Clear()
      .Set(Help::Valid(), _("SELECT"))
      .Set(HelpType::Start, _("VALIDATE"))
      .Set(Help::Cancel(), _("CANCEL"))
      .Set(HelpType::X, _("DELETE"))
      .Set(HelpType::Y, _("BACKSPACE"))
      .Set(HelpType::LR, _("MOVE CURSOR"))
      .Set(HelpType::L2R2, _("FAST WHEEL"))
      .Set(HelpType::UpDown, _("CHANGE CHARSET"))
      .Set(HelpType::LeftRight, _("MOVE WHEEL"))
      //.Set(HelpType::Joy1LeftRight, _("FAST WHEEL"))
      .Set(HelpType::Joy2LeftRight, _("FAST CURSOR"));
  return true;
}


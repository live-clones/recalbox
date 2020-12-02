//
// Created by bkg2k on 12/09/2020.
//
#pragma once

#include <utils/math/Vector2f.h>

class Rectangle
{
  public:
    Rectangle()
      : mX(0)
      , mY(0)
      , mW(0)
      , mH(0)
    {
    }

    explicit Rectangle(const Vector2f& size)
      : mX(0)
      , mY(0)
      , mW(size.x())
      , mH(size.y())
    {
    }

    Rectangle(float width, float height)
      : mX(0)
      , mY(0)
      , mW(width)
      , mH(height)
    {
    }

    Rectangle(const Vector2f& topleft, const Vector2f& widthheight)
      : mX(topleft.x())
      , mY(topleft.y())
      , mW(widthheight.x())
      , mH(widthheight.y())
    {
    }

    Rectangle(float x, float y, float width, float height)
      : mX(x)
      , mY(y)
      , mW(width)
      , mH(height)
    {
    }

    float Left()   const { return mX; }
    float Top()    const { return mY; }
    float Right()  const { return mX + mW - 1; }
    float Bottom() const { return mY + mH - 1; }
    float Width()  const { return mW; }
    float Height() const { return mH; }

    Rectangle& Contract(const Vector2f& by)
    {
      mX += by.x();
      mY += by.y();
      mW -= by.x() * 2;
      mH -= by.y() * 2;

      if (mW < 0) { mX += mW / 2; mW = 0; }
      if (mH < 0) { mY += mH / 2; mH = 0; }

      return *this;
    }

    Rectangle& Expand(const Vector2f& by)
    {
      mX -= by.x();
      mY -= by.y();
      mW += by.x() * 2;
      mH += by.y() * 2;

      if (mW < 0) { mX += mW / 2; mW = 0; }
      if (mH < 0) { mY += mH / 2; mH = 0; }

      return *this;
    }

    Rectangle& Contract(float deltaX, float deltaY)
    {
      mX += deltaX;
      mY += deltaY;
      mW -= deltaX * 2;
      mH -= deltaY * 2;

      if (mW < 0) { mX += mW / 2; mW = 0; }
      if (mH < 0) { mY += mH / 2; mH = 0; }

      return *this;
    }

    Rectangle& Expand(float deltaX, float deltaY)
    {
      mX -= deltaX;
      mY -= deltaY;
      mW += deltaX * 2;
      mH += deltaY * 2;

      if (mW < 0) { mX += mW / 2; mW = 0; }
      if (mH < 0) { mY += mH / 2; mH = 0; }

      return *this;
    }

    Rectangle& Move(const Vector2f& by)
    {
      mX += by.x();
      mY += by.y();
      return *this;
    }

    Rectangle& Move(float deltaX, float deltaY)
    {
      mX += deltaX;
      mY += deltaY;
      return *this;
    }

    Rectangle& MoveLeft(float delta)
    {
      mX += delta;
      if ((mW -= delta) < 0.0f) mW = 0.0f;
      return *this;
    }

    Rectangle& MoveTop(float delta)
    {
      mY += delta;
      if ((mH -= delta) < 0.0f) mH = 0.0f;
      return *this;
    }

    Rectangle& MoveRight(float delta)
    {
      if ((mW += delta) < 0.0f) mW = 0.0f;
      return *this;
    }

    Rectangle& MoveBottom(float delta)
    {
      if ((mH += delta) < 0.0f) mH = 0.0f;
      return *this;
    }

    Rectangle& ResetReference()
    {
      mX = mY = 0;
      return *this;
    }

    bool operator ==(const Rectangle& r) const
    {
      return ((mX == r.mX) && (mY == r.mY) && (mW == r.mW) && (mH == r.mH));
    }

    bool operator !=(const Rectangle& r) const
    {
      return ((mX != r.mX) || (mY != r.mY) || (mW != r.mW) || (mH != r.mH));
    }

    Rectangle& MirrorY(int screenHeight)
    {
      mY = (float)screenHeight - mY - mH;
      return *this;
    }

  private:
    float mX;
    float mY;
    float mW;
    float mH;
};

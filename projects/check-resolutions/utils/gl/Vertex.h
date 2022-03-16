//
// Created by bkg2k on 14/09/2020.
//
#pragma once

#include <utils/math/Vector2f.h>
#include <utils/math/Misc.h>

//! GL vertex
struct Vertex
{
  public:
    static constexpr int sVertexPerTriangle = 3;
    static constexpr int sVertexPerRectangle = sVertexPerTriangle * 2;

    struct Point
    {
      public:
        float X;
        float Y;

        Point& operator  = (const Vector2f& v) { X = v.x(); Y = v.y(); return *this; }
        Point& operator += (const Vector2f& v) { X += v.x(); Y += v.y(); return *this; }
        Point  operator  + (const Vector2f& v) const { return { X + v.x(), Y + v.y() }; }
        void Set(float x, float y) { X = x; Y= y; }
        void Round() { X = Math::round(X); Y = Math::round(Y); }
    };

    Point Target; //<! Target (Screen) coordinates
    Point Source; //<! Source (Texture) coordinates
};
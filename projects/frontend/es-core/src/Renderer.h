#pragma once

#include "platform_gl.h"
#include <stack>
#include <SDL.h>
#include <utils/math/Vector4i.h>
#include <utils/math/Vector2i.h>
#include <utils/math/Transform4x4f.h>

class Component;
class Font;

//The Renderer provides several higher-level functions for drawing (rectangles, text, etc.).
//Renderer_draw_gl.cpp has most of the higher-level functions and wrappers.
//Renderer_init_*.cpp has platform-specific renderer initialziation/deinitialziation code.  (e.g. the Raspberry Pi sets up dispmanx/OpenGL ES)
class Renderer
{
  private:
    static bool sInitialCursorState;

    static SDL_Window* sSdlWindow;
    static SDL_GLContext sSdlContext;

    static int sDisplayWidth;
    static int sDisplayHeight;
    static float sDisplayWidthFloat;
    static float sDisplayHeightFloat;

    static std::stack<Vector4i>& ClipStack();

    static void setColor4bArray(GLubyte* array, unsigned int color);

    static bool createSurface();
    static void destroySurface();

  public:

    static bool initialize(int w, int h);
    static void finalize();

    static void drawRect(int x, int y, int w, int h, unsigned int color, GLenum blend_sfactor = GL_SRC_ALPHA, GLenum blend_dfactor = GL_ONE_MINUS_SRC_ALPHA);
    static void drawRect(float x, float y, float w, float h, unsigned int color, GLenum blend_sfactor = GL_SRC_ALPHA, GLenum blend_dfactor = GL_ONE_MINUS_SRC_ALPHA);

    static void buildGLColorArray(GLubyte* ptr, unsigned int color, unsigned int vertCount);
    static void setMatrix(const Transform4x4f& transform);

    static void pushClipRect(Vector2i pos, Vector2i dim);
    static void popClipRect();

    static void swapBuffers();

    static int getDisplayWidthAsInt() { return sDisplayWidth; }
    static int getDisplayHeightAsInt() { return sDisplayHeight; }
    static float getDisplayWidthAsFloat() { return sDisplayWidthFloat; }
    static float getDisplayHeightAsFloat() { return sDisplayHeightFloat; }

    static bool IsInitialized() { return sSdlWindow != nullptr; }
};

#pragma once

#include <vector>
#include <string>
#include <stack>
#include <SDL.h>
#include "platform_gl.h"
#include "utils/math/Vectors.h"

class GuiComponent;
class Font;

//The Renderer provides several higher-level functions for drawing (rectangles, text, etc.).
//Renderer_draw_gl.cpp has most of the higher-level functions and wrappers.
//Renderer_init_*.cpp has platform-specific renderer initialziation/deinitialziation code.  (e.g. the Raspberry Pi sets up dispmanx/OpenGL ES)
class Renderer
{
  private:
    static bool initialCursorState;

    static SDL_Window* sdlWindow;
    static SDL_GLContext sdlContext;

    static int _DisplayWidth;
    static int _DisplayHeight;
    static float _DisplayWidthFloat;
    static float _DisplayHeightFloat;

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

    static int getDisplayWidthAsInt() { return _DisplayWidth; }
    static int getDisplayHeightAsInt() { return _DisplayHeight; }
    static float getDisplayWidthAsFloat() { return _DisplayWidthFloat; }
    static float getDisplayHeightAsFloat() { return _DisplayHeightFloat; }
};

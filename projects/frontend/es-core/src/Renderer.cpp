#include "Renderer.h"
#include <iostream>
#include <stack>
#include "platform.h"
#include "platform_gl.h"
#include "Log.h"
#include "ImageIO.h"
#include "../data/Resources.h"
#include "Settings.h"

#ifdef USE_OPENGL_ES
  #define glOrtho glOrthof
#endif

bool Renderer::initialCursorState = false;

SDL_Window* Renderer::sdlWindow = nullptr;
SDL_GLContext Renderer::sdlContext = nullptr;

int Renderer::_DisplayWidth = 0;
int Renderer::_DisplayHeight = 0;
float Renderer::_DisplayWidthFloat = 0.0f;
float Renderer::_DisplayHeightFloat = 0.0f;

bool Renderer::createSurface()
{
  LOG(LogInfo) << "Creating surface...";

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    LOG(LogError) << "Error initializing SDL!\n	" << SDL_GetError();
    return false;
  }

  //hide mouse cursor
  initialCursorState = SDL_ShowCursor(0) == 1;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // multisample anti-aliasing
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

  #ifdef USE_OPENGL_ES
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  #endif

  SDL_DisplayMode dispMode;
  SDL_GetDesktopDisplayMode(0, &dispMode);
  if (_DisplayWidth == 0)  _DisplayWidth = dispMode.w;
  if (_DisplayHeight == 0) _DisplayHeight = dispMode.h;
  _DisplayWidthFloat = (float)_DisplayWidth;
  _DisplayHeightFloat = (float)_DisplayHeight;

  sdlWindow = SDL_CreateWindow("EmulationStation",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               _DisplayWidth, _DisplayHeight,
                               SDL_WINDOW_OPENGL | (Settings::getInstance()->getBool("Windowed") ? 0 : SDL_WINDOW_FULLSCREEN));

  if (sdlWindow == nullptr)
  {
    LOG(LogError) << "Error creating SDL window!\n\t" << SDL_GetError();
    return false;
  }

  LOG(LogInfo) << "Created window successfully.";

  //set an icon for the window
  size_t width = 0;
  size_t height = 0;
  std::vector<unsigned char> rawData = ImageIO::loadFromMemoryRGBA32(window_icon_256_png_data, window_icon_256_png_size,
                                                                     width, height);
  if (!rawData.empty())
  {
    ImageIO::flipPixelsVert(rawData.data(), width, height);

    //SDL interprets each pixel as a 32-bit number, so our masks must depend on the endianness (byte order) of the machine
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000; Uint32 gmask = 0x00ff0000; Uint32 bmask = 0x0000ff00; Uint32 amask = 0x000000ff;
    #else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
    #endif
    //try creating SDL surface from logo data
    SDL_Surface* logoSurface = SDL_CreateRGBSurfaceFrom((void*) rawData.data(), width, height, 32, (int) (width * 4),
                                                        rmask, gmask, bmask, amask);
    if (logoSurface != nullptr)
    {
      SDL_SetWindowIcon(sdlWindow, logoSurface);
      SDL_FreeSurface(logoSurface);
    }
  }

  sdlContext = SDL_GL_CreateContext(sdlWindow);

  // vsync
  if (Settings::getInstance()->getBool("VSync"))
  {
    // SDL_GL_SetSwapInterval(0) for immediate updates (no vsync, default),
    // 1 for updates synchronized with the vertical retrace,
    // or -1 for late swap tearing.
    // SDL_GL_SetSwapInterval returns 0 on success, -1 on error.
    // if vsync is requested, try late swap tearing; if that doesn't work, try normal vsync
    // if that doesn't work, report an error
    if (SDL_GL_SetSwapInterval(-1) != 0 && SDL_GL_SetSwapInterval(1) != 0)
    {
      LOG(LogWarning) << "Tried to enable vsync, but failed! (" << SDL_GetError() << ")";
    }
  }

  return true;
}

void Renderer::swapBuffers()
{
  SDL_GL_SwapWindow(sdlWindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::destroySurface()
{
  SDL_GL_DeleteContext(sdlContext);
  sdlContext = nullptr;

  SDL_DestroyWindow(sdlWindow);
  sdlWindow = nullptr;

  //show mouse cursor
  SDL_ShowCursor(initialCursorState);

  SDL_Quit();
}

bool Renderer::initialize(int w, int h)
{
  if (w)
    _DisplayWidth = w;
  if (h)
    _DisplayHeight = h;

  bool createdSurface = createSurface();

  if (!createdSurface)
    return false;

  glViewport(0, 0, _DisplayWidth, _DisplayHeight);

  glMatrixMode(GL_PROJECTION);
  glOrtho(0, _DisplayWidth, _DisplayHeight, 0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  return true;
}

void Renderer::finalize()
{
  destroySurface();
}

std::stack<Vector4i>& Renderer::ClipStack()
{
  static std::stack<Vector4i> _ClippingStack;
  return _ClippingStack;
}

void Renderer::setColor4bArray(GLubyte* array, unsigned int color)
{
  array[0] = (color & 0xff000000) >> 24;
  array[1] = (color & 0x00ff0000) >> 16;
  array[2] = (color & 0x0000ff00) >> 8;
  array[3] = (color & 0x000000ff);
}

void Renderer::buildGLColorArray(GLubyte* ptr, unsigned int color, unsigned int vertCount)
{
  unsigned int colorGl;
  setColor4bArray((GLubyte*) &colorGl, color);
  for (unsigned int i = 0; i < vertCount; i++)
  {
    ((GLuint*) ptr)[i] = colorGl;
  }
}

void Renderer::pushClipRect(Vector2i pos, Vector2i dim)
{
  Vector4i box(pos.x(), pos.y(), dim.x(), dim.y());
  if (box[2] == 0)
    box[2] = Renderer::_DisplayWidth - box.x();
  if (box[3] == 0)
    box[3] = Renderer::_DisplayHeight - box.y();

  //glScissor starts at the bottom left of the window
  //so (0, 0, 1, 1) is the bottom left pixel
  //everything else uses y+ = down, so flip it to be consistent
  //rect.pos.y = Renderer::getScreenHeight() - rect.pos.y - rect.size.y;
  box[1] = Renderer::_DisplayHeight - box.y() - box[3];

  //make sure the box fits within clipStack.top(), and clip further accordingly
  if (!ClipStack().empty())
  {
    Vector4i& top = ClipStack().top();
    if (top[0] > box[0])
      box[0] = top[0];
    if (top[1] > box[1])
      box[1] = top[1];
    if (top[0] + top[2] < box[0] + box[2])
      box[2] = (top[0] + top[2]) - box[0];
    if (top[1] + top[3] < box[1] + box[3])
      box[3] = (top[1] + top[3]) - box[1];
  }

  if (box[2] < 0)
    box[2] = 0;
  if (box[3] < 0)
    box[3] = 0;

  ClipStack().push(box);
  glScissor(box[0], box[1], box[2], box[3]);
  glEnable(GL_SCISSOR_TEST);
}

void Renderer::popClipRect()
{
  if (ClipStack().empty())
  {
    LOG(LogError) << "Tried to popClipRect while the stack was empty!";
    return;
  }

  ClipStack().pop();
  if (ClipStack().empty())
  {
    glDisable(GL_SCISSOR_TEST);
  }
  else
  {
    Vector4i top = ClipStack().top();
    glScissor(top[0], top[1], top[2], top[3]);
  }
}

void
Renderer::drawRect(float x, float y, float w, float h, unsigned int color, GLenum blend_sfactor, GLenum blend_dfactor)
{
  drawRect((int) round(x), (int) round(y), (int) round(w), (int) round(h), color, blend_sfactor, blend_dfactor);
}

void Renderer::drawRect(int x, int y, int w, int h, unsigned int color, GLenum blend_sfactor, GLenum blend_dfactor)
{
  #ifdef USE_OPENGL_ES
  GLshort points[12];
  #else
  GLint points[12];
  #endif

  points[0] = x;
  points[1] = y;
  points[2] = x;
  points[3] = y + h;
  points[4] = x + w;
  points[5] = y;

  points[6] = x + w;
  points[7] = y;
  points[8] = x;
  points[9] = y + h;
  points[10] = x + w;
  points[11] = y + h;

  GLubyte colors[6 * 4];
  buildGLColorArray(colors, color, 6);

  glEnable(GL_BLEND);
  glBlendFunc(blend_sfactor, blend_dfactor);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  #ifdef USE_OPENGL_ES
  glVertexPointer(2, GL_SHORT, 0, points);
  #else
  glVertexPointer(2, GL_INT, 0, points);
  #endif
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisable(GL_BLEND);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

void Renderer::setMatrix(const Transform4x4f& matrix)
{
  glLoadMatrixf((float*) &matrix);
}

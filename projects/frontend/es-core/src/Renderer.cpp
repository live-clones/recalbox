#include <utils/math/Misc.h>
#include "Renderer.h"
#include "platform_gl.h"
#include "utils/Log.h"
#include "ImageIO.h"
#include "../data/Resources.h"
#include <RecalboxConf.h>

#ifdef USE_OPENGL_ES
  #define glOrtho glOrthof
#endif

#ifdef DEBUG

static void APIENTRY GLDebugCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar* message,
                                     const void* userParam)
{
  (void)source;
  (void)length;
  (void)userParam;

  const char* typeString = "UNKNOWN";
  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR: typeString = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeString = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY: typeString = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: typeString = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER: typeString = "OTHER"; break;
    default: break;
  }

  const char* severityString = "UNKNOWN";
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_LOW: severityString = "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: severityString = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH: severityString = "HIGH"; break;
    default: break;
  }

  { LOG(LogError) << "[Renderer] GL Error [Type:" << typeString << " - Severity:" << severityString << "] {id:" << id << "} " << message; }
}

#endif

void Renderer::ActivateGLDebug()
{
  #ifdef DEBUG
  { LOG(LogInfo) << "[Renderer] GL Debug activated."; }
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(GLDebugCallback, nullptr);
  GLuint unusedIds = 0;
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, 1);
  #endif
}

Renderer::Renderer(int width, int height, bool windowed)
  : StaticLifeCycleControler<Renderer>("Renderer")
  , mSdlWindow(nullptr)
  , mSdlGLContext(nullptr)
  , mDisplayWidth(0)
  , mDisplayHeight(0)
  , mDisplayWidthFloat(0.0f)
  , mDisplayHeightFloat(0.0f)
  , mViewPortInitialized(false)
  , mInitialCursorState(false)
  , mWindowed(windowed)
{
  #ifdef DEBUG
  ActivateGLDebug();
  #endif

  mViewPortInitialized = Initialize(width, height);
}

Renderer::~Renderer()
{
  Finalize();
}


bool Renderer::CreateSdlSurface()
{
  { LOG(LogInfo) << "[Renderer] Creating surface..."; }

  /*if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
  {
    { LOG(LogError) << "[Renderer] Error initializing SDL!\n	" << SDL_GetError(); }
    return false;
  }*/

  //hide mouse cursor
  mInitialCursorState = SDL_ShowCursor(0) == 1;

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
  if (mDisplayWidth == 0)  mDisplayWidth = dispMode.w;
  if (mDisplayHeight == 0) mDisplayHeight = dispMode.h;
  mDisplayWidthFloat = (float)mDisplayWidth;
  mDisplayHeightFloat = (float)mDisplayHeight;
  { LOG(LogInfo) << "[Renderer] Resolution: " << mDisplayWidth << ',' << mDisplayHeight; }

  mSdlWindow = SDL_CreateWindow("EmulationStation",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                mDisplayWidth,
                                mDisplayHeight,
                                SDL_WINDOW_OPENGL | (mWindowed ? 0 : SDL_WINDOW_FULLSCREEN));

  if (mSdlWindow == nullptr)
  {
    { LOG(LogError) << "[Renderer] Error creating SDL window!\n\t" << SDL_GetError(); }
    return false;
  }

  { LOG(LogInfo) << "[Renderer] Created window successfully."; }

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
      SDL_SetWindowIcon(mSdlWindow, logoSurface);
      SDL_FreeSurface(logoSurface);
    }
  }

  mSdlGLContext = SDL_GL_CreateContext(mSdlWindow);

  // vsync
  { LOG(LogInfo) << "[Renderer] Activating vertical sync'"; }
  // SDL_GL_SetSwapInterval(0) for immediate updates (no vsync, default),
  // 1 for updates synchronized with the vertical retrace,
  // or -1 for late swap tearing.
  // SDL_GL_SetSwapInterval returns 0 on success, -1 on error.
  // if vsync is requested, try late swap tearing; if that doesn't work, try normal vsync
  // if that doesn't work, report an error
  /*if (SDL_GL_SetSwapInterval(-1) == 0) LOG(LogInfo) << "Adaptative VSync' activated.";
  else*/
  if (SDL_GL_SetSwapInterval(1) == 0) { LOG(LogInfo) << "[Renderer] Normal VSync' activated."; }
  else { LOG(LogWarning) << "[Renderer] Tried to enable vsync, but failed! (" << SDL_GetError() << ")"; }

  return true;
}

void Renderer::SwapBuffers()
{
  SDL_GL_SwapWindow(mSdlWindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DestroySdlSurface()
{
  SDL_GL_DeleteContext(mSdlGLContext);
  mSdlGLContext = nullptr;

  SDL_DestroyWindow(mSdlWindow);
  mSdlWindow = nullptr;

  //show mouse cursor
  SDL_ShowCursor(mInitialCursorState ? 1 : 0);

  //SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool Renderer::Initialize(int w, int h)
{
  if (w != 0) mDisplayWidth = w;
  if (h != 0) mDisplayHeight = h;

  bool createdSurface = CreateSdlSurface();
  if (!createdSurface) return false;

  glViewport(0, 0, mDisplayWidth, mDisplayHeight);

  glMatrixMode(GL_PROJECTION);
  glOrtho(0, mDisplayWidth, mDisplayHeight, 0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  return true;
}

void Renderer::Finalize()
{
  DestroySdlSurface();
}

void Renderer::BuildGLColorArray(GLubyte* ptr, Colors::ColorARGB color, int vertCount)
{
  unsigned int colorGl = 0;
  ColorToByteArray((GLubyte*) &colorGl, color);
  for (int i = vertCount; --i >= 0; )
    ((GLuint*) ptr)[i] = colorGl;
}

void Renderer::PushClippingRect(Vector2i pos, Vector2i dim)
{
  Vector4i box(pos.x(), pos.y(), dim.x(), dim.y());
  if (box[2] == 0)
    box[2] = mDisplayWidth - box.x();
  if (box[3] == 0)
    box[3] = mDisplayHeight - box.y();

  //glScissor starts at the bottom left of the window
  //so (0, 0, 1, 1) is the bottom left pixel
  //everything else uses y+ = down, so flip it to be consistent
  //rect.pos.y = Renderer::Instance().GetScreenHeight() - rect.pos.y - rect.size.y;
  box[1] = mDisplayHeight - box.y() - box[3];

  //make sure the box fits within clipStack.top(), and clip further accordingly
  if (!mClippingStack.empty())
  {
    Vector4i& top = mClippingStack.top();
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

  mClippingStack.push(box);
  glScissor(box[0], box[1], box[2], box[3]);
  glEnable(GL_SCISSOR_TEST);
}

void Renderer::Clip(const Rectangle& area)
{
  (void)area;
}

void Renderer::Unclip()
{

}

void Renderer::PopClippingRect()
{
  if (mClippingStack.empty())
  {
    { LOG(LogError) << "[Renderer] Tried to popClipRect while the stack was empty!"; }
    return;
  }

  mClippingStack.pop();
  if (mClippingStack.empty())
  {
    glDisable(GL_SCISSOR_TEST);
  }
  else
  {
    Vector4i top = mClippingStack.top();
    glScissor(top[0], top[1], top[2], top[3]);
  }
}

GLuint Renderer::CreateGLTexture()
{
  GLuint id = 0;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return id;
}

void Renderer::DestroyGLTexture(GLuint id)
{
  glDeleteTextures(1, &id);
}

void Renderer::DrawRectangle(const Rectangle& area, Colors::ColorARGB color, GLenum blend_sfactor, GLenum blend_dfactor)
{
  DrawRectangle(Math::roundi(area.Left()), Math::roundi(area.Top()),
                Math::roundi(area.Width()), Math::roundi(area.Height()),
                color, blend_sfactor, blend_dfactor);
}

void
Renderer::DrawRectangle(float x, float y, float w, float h, Colors::ColorARGB color, GLenum blend_sfactor, GLenum blend_dfactor)
{
  DrawRectangle(Math::roundi(x), Math::roundi(y), Math::roundi(w), Math::roundi(h), color, blend_sfactor, blend_dfactor);
}

void Renderer::DrawRectangle(int x, int y, int w, int h, Colors::ColorARGB color, GLenum blend_sfactor, GLenum blend_dfactor)
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
  BuildGLColorArray(colors, color, 6);

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

void Renderer::SetMatrix(const Transform4x4f& transform)
{
  glLoadMatrixf((float*) &transform);
}

Renderer::Error Renderer::UploadAlpha(GLuint id, int width, int height, const void* data)
{
  glBindTexture(GL_TEXTURE_2D, id);
  if (glGetError() != GL_NO_ERROR) return Error::NoResource;

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
  if (glGetError() == GL_OUT_OF_MEMORY) return Error::OutOfGPUMemory;

  return Error::NoError;
}

Renderer::Error Renderer::UploadRGBA(GLuint id, int width, int height, const void* data)
{
  glBindTexture(GL_TEXTURE_2D, id);
  if (glGetError() != GL_NO_ERROR) return Error::NoResource;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  if (glGetError() == GL_OUT_OF_MEMORY) return Error::OutOfGPUMemory;

  return Error::NoError;
}

Renderer::Error Renderer::UploadAlphaPart(GLuint id, int x, int y, int width, int height, const void* data)
{
  glBindTexture(GL_TEXTURE_2D, id);
  if (glGetError() != GL_NO_ERROR) return Error::NoResource;

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, data);
  if (glGetError() == GL_OUT_OF_MEMORY) return Error::OutOfGPUMemory;

  return Error::NoError;
}

Renderer::Error Renderer::UploadRGBAPart(GLuint id, int x, int y, int width, int height, const void* data)
{
  glBindTexture(GL_TEXTURE_2D, id);
  if (glGetError() != GL_NO_ERROR) return Error::NoResource;

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  if (glGetError() == GL_OUT_OF_MEMORY) return Error::OutOfGPUMemory;

  return Error::NoError;
}

void Renderer::DrawLines(const Vector2f coordinates[], const Colors::ColorARGB colors[], int count)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glVertexPointer(2, GL_FLOAT, 0, &coordinates);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors);

  glDrawArrays(GL_LINES, 0, count);

  glDisable(GL_BLEND);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

void Renderer::DrawTexturedTriangles(GLuint id, const Vertex vertices[], const GLubyte colors[], int count, bool tiled)
{
  if (id != 0)
    glBindTexture(GL_TEXTURE_2D, id);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tiled ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tiled ? GL_REPEAT : GL_CLAMP_TO_EDGE);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].Target);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].Source);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

  glDrawArrays(GL_TRIANGLES, 0, count);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void Renderer::DrawTexturedTriangles(GLuint id, const Vertex vertices[], Colors::ColorARGB color, int count, bool tiled)
{
  if (id != 0)
    glBindTexture(GL_TEXTURE_2D, id);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tiled ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tiled ? GL_REPEAT : GL_CLAMP_TO_EDGE);

  glColor4ub((GLubyte)(color >> 24),
             (GLubyte)(color >> 16),
             (GLubyte)(color >> 8),
             (GLubyte)(color >> 0));

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].Target);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].Source);

  glDrawArrays(GL_TRIANGLES, 0, count);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}


#include <utils/math/Misc.h>
#include "Renderer.h"
#include "platform_gl.h"
#include "utils/Log.h"
#include "ImageIO.h"
#include "../data/Resources.h"
#include "CrtConf.h"
#include <RecalboxConf.h>
#include <hardware/Board.h>

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
  , mVirtualDisplayWidth(0)
  , mVirtualDisplayWidthFloat(0.0f)
  , mScale(0.0f)
  , mAspectRatio(0.0f)
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


bool Renderer::CreateSdlSurface(int width, int height)
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

  mDisplayWidth = width;
  mDisplayHeight = height;
  mDisplayWidthFloat = (float)mDisplayWidth;
  mDisplayHeightFloat = (float)mDisplayHeight;
  { LOG(LogInfo) << "[Renderer] Trying Resolution: " << width << ',' << height; }

  int flags = SDL_WINDOW_OPENGL;
  if (!mWindowed) flags |= Board::Instance().GetBoardType() == BoardType::PCx64 ?
                           SDL_WINDOW_FULLSCREEN_DESKTOP :
                           SDL_WINDOW_FULLSCREEN;
  mSdlWindow = SDL_CreateWindow("EmulationStation",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                mDisplayWidth,
                                mDisplayHeight,
                                flags);

  if (mSdlWindow == nullptr)
  {
    { LOG(LogError) << "[Renderer] Error creating SDL window!\n\t" << SDL_GetError(); }
    return false;
  }

  { LOG(LogInfo) << "[Renderer] Created window successfully."; }

  //set an icon for the window
  size_t icwidth = 0;
  size_t icheight = 0;
  std::vector<unsigned char> rawData = ImageIO::loadFromMemoryRGBA32(window_icon_256_png_data, window_icon_256_png_size,
                                                                     icwidth, icheight);
  if (!rawData.empty())
  {
    ImageIO::flipPixelsVert(rawData.data(), icwidth, icheight);

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
    SDL_Surface* logoSurface = SDL_CreateRGBSurfaceFrom((void*) rawData.data(), (int)icwidth, (int)icheight, 32, (int)(icwidth * 4),
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

bool Renderer::GetResolutionFromString(const std::string& resolution, int& w, int& h)
{
  //! Small resolution structure
  struct Resolution
  {
    int Width;  //!< Width in pixels
    int Height; //!< Height in pixels
  };

  static HashMap<int, Resolution> CeaModes
  ({
    { 1, { 640, 480 } },
    { 2, { 640, 480 } },
    { 3, { 854, 480 } },
    { 4, { 1280, 720 } },
    { 5, { 1920, 1080 } },
    { 6, { 640, 480 } },
    { 7, { 854, 480 } },
    { 8, { 320, 240 } },
    { 9, { 426, 240 } },
    { 10, { 640, 480 } },
    { 11, { 854, 480 } },
    { 12, { 320, 240 } },
    { 13, { 426, 240 } },
    { 14, { 640, 480 } },
    { 15, { 854, 480 } },
    { 16, { 1920, 1080 } },
    { 17, { 768, 576 } },
    { 18, { 1024, 576 } },
    { 19, { 1280, 720 } },
    { 20, { 1920, 1080 } },
    { 21, { 768, 576 } },
    { 22, { 1024, 576 } },
    { 23, { 384, 288 } },
    { 24, { 512, 288 } },
    { 25, { 768, 576 } },
    { 26, { 1024, 576 } },
    { 27, { 384, 288 } },
    { 28, { 512, 288 } },
    { 29, { 768, 576 } },
    { 30, { 1024, 576 } },
    { 31, { 1920, 1080 } },
    { 32, { 1920, 1080 } },
    { 33, { 1920, 1080 } },
    { 34, { 1920, 1080 } },
    { 35, { 640, 480 } },
    { 36, { 854, 480 } },
    { 37, { 768, 576 } },
    { 38, { 1024, 576 } },
    { 39, { 1920, 1080 } },
    { 40, { 1920, 1080 } },
    { 41, { 1280, 720 } },
    { 42, { 768, 576 } },
    { 43, { 1024, 576 } },
    { 44, { 768, 576 } },
    { 45, { 1024, 576 } },
    { 46, { 1920, 1080 } },
    { 47, { 1280, 720 } },
    { 48, { 640, 480 } },
    { 49, { 854, 480 } },
    { 50, { 640, 480 } },
    { 51, { 854, 480 } },
    { 52, { 768, 576 } },
    { 53, { 1024, 576 } },
    { 54, { 768, 576 } },
    { 55, { 1024, 576 } },
    { 56, { 640, 480 } },
    { 57, { 854, 480 } },
    { 58, { 640, 480 } },
    { 59, { 854, 480 } },
    { 60, { 1280, 720 } },
    { 61, { 1280, 720 } },
    { 62, { 1280, 720 } },
    { 63, { 1920, 1080 } },
    { 64, { 1920, 1080 } },
  });

  static HashMap<int, Resolution> DmtModes
  ({
    { 1, { 640, 350 } },
    { 2, { 640, 400 } },
    { 3, { 720, 400 } },
    { 4, { 640, 480 } },
    { 5, { 640, 480 } },
    { 6, { 640, 480 } },
    { 7, { 640, 480 } },
    { 8, { 800, 600 } },
    { 9, { 800, 600 } },
    { 10, { 800, 600 } },
    { 11, { 800, 600 } },
    { 12, { 800, 600 } },
    { 13, { 800, 600 } },
    { 14, { 848, 480 } },
    { 15, { 1024, 768 } },
    { 16, { 1024, 768 } },
    { 17, { 1024, 768 } },
    { 18, { 1024, 768 } },
    { 19, { 1024, 768 } },
    { 20, { 1024, 768 } },
    { 21, { 1152, 864 } },
    { 22, { 1280, 768 } },
    { 23, { 1280, 768 } },
    { 24, { 1280, 768 } },
    { 25, { 1280, 768 } },
    { 26, { 1280, 768 } },
    { 27, { 1280, 800 } },
    { 28, { 1280, 800 } },
    { 29, { 1280, 800 } },
    { 30, { 1280, 800 } },
    { 31, { 1280, 800 } },
    { 32, { 1280, 960 } },
    { 33, { 1280, 960 } },
    { 34, { 1280, 960 } },
    { 35, { 1280, 1024 } },
    { 36, { 1280, 1024 } },
    { 37, { 1280, 1024 } },
    { 38, { 1280, 1024 } },
    { 39, { 1360, 768 } },
    { 40, { 1360, 768 } },
    { 41, { 1400, 1050 } },
    { 42, { 1400, 1050 } },
    { 43, { 1400, 1050 } },
    { 44, { 1400, 1050 } },
    { 45, { 1400, 1050 } },
    { 46, { 1440, 900 } },
    { 47, { 1440, 900 } },
    { 48, { 1440, 900 } },
    { 49, { 1440, 900 } },
    { 50, { 1440, 900 } },
    { 51, { 1600, 1200 } },
    { 52, { 1600, 1200 } },
    { 53, { 1600, 1200 } },
    { 54, { 1600, 1200 } },
    { 55, { 1600, 1200 } },
    { 56, { 1600, 1200 } },
    { 57, { 1680, 1050 } },
    { 58, { 1680, 1050 } },
    { 59, { 1680, 1050 } },
    { 60, { 1680, 1050 } },
    { 61, { 1680, 1050 } },
    { 62, { 1792, 1344 } },
    { 63, { 1792, 1344 } },
    { 64, { 1792, 1344 } },
    { 65, { 1856, 1392 } },
    { 66, { 1856, 1392 } },
    { 67, { 1856, 1392 } },
    { 68, { 1920, 1200 } },
    { 69, { 1920, 1200 } },
    { 70, { 1920, 1200 } },
    { 71, { 1920, 1200 } },
    { 72, { 1920, 1200 } },
    { 73, { 1920, 1440 } },
    { 74, { 1920, 1440 } },
    { 75, { 1920, 1440 } },
    { 76, { 2560, 1600 } },
    { 77, { 2560, 1600 } },
    { 78, { 2560, 1600 } },
    { 79, { 2560, 1600 } },
    { 80, { 2560, 1600 } },
    { 81, { 1366, 768 } },
    { 82, { 1920, 1080 } },
    { 83, { 1600, 900 } },
    { 84, { 2048, 1152 } },
    { 85, { 1280, 720 } },
    { 86, { 1366, 768 } },
  });

  static HashMap<std::string, Resolution> Named
  ({
    { "nhd" , { 640, 360 } },
    { "qhd" , { 960, 540 } },
    { "hd" , { 1280, 720 } },
    { "hd+" , { 1600, 900 } },
    { "fhd" , { 1920, 1080 } },
    { "wqhd" , { 2560, 1440 } },
    { "qhd+" , { 3200, 1800 } },
    { "4k uhd" , { 3840, 2160 } },

    { "qqvga" , { 160, 120 } },
    { "hqvga" , { 240, 160 } },
    { "qvga" , { 320, 240 } },
    { "wqvga" , { 384, 240 } },
    { "hvga" , { 480, 320 } },
    { "vga" , { 640, 480 } },
    { "wvga" , { 768, 480 } },
    { "fwvga" , { 854, 480 } },
    { "svga" , { 800, 600 } },
    { "wsvga" , { 1024, 576 } },
    { "dvga" , { 960, 640 } },

    { "xga" , { 1024, 768 } },
    { "wxga" , { 1280, 768 } },
    { "fwxga" , { 1366, 768 } },
    { "xga+" , { 1152, 864 } },
    { "wxga+" , { 1440, 900 } },
    { "sxga" , { 1280, 1024 } },
    { "wsxga" , { 1440, 960 } },
    { "sxga+" , { 1400, 1050 } },
    { "wsxga+" , { 1680, 1050 } },
    { "uxga" , { 1600, 1200 } },
    { "wuxga" , { 1920, 1200 } },

    { "qwxga" , { 2048, 1152 } },
    { "qxga" , { 2048, 1536 } },
    { "wqxga" , { 2560, 1600 } },
    { "qsxga" , { 2560, 2048 } },
    { "wqsxga" , { 3200, 2048 } },
    { "quxga" , { 3200, 2400 } },
    { "wquxga" , { 3840, 2400 } },
  });

  // Named resolution?
  Resolution* reso = Named.try_get(resolution);
  if (reso != nullptr)
  {
    w = reso->Width;
    h = reso->Height;
    return true;
  }

  // Width x Height?
  std::string swidth, sheight;
  int width = 0, height = 0;
  if (Strings::SplitAt(resolution, 'x', swidth, sheight, true))
    if (Strings::ToInt(swidth, width) && Strings::ToInt(sheight, height))
    {
      w = width;
      h = height;
      return true;
    }

  // Raspberry syntax?
  std::string mode, sindex, tail;
  int index =0;
  if (Strings::SplitAt(resolution, ' ', mode, sindex, true))
    if (Strings::ToInt(sindex, 0, ' ', index))
    {
      if (mode == "cea") reso = CeaModes.try_get(index);
      if (mode == "dmt") reso = DmtModes.try_get(index);
      if (reso != nullptr)
      {
        w = reso->Width;
        h = reso->Height;
        return true;
      }
    }

  return false;
}

void Renderer::GetResolutionFromConfiguration(int& w, int& h)
{
  switch(Board::Instance().GetBoardType())
  {
    // Fixed resolutions and/or tvservice available
    case BoardType::Pi0:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper: break;
    // Variable resolution and/or tvservice unavailable
    case BoardType::Pi02:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default:
    {
      std::string resolution = Strings::Trim(Strings::ToLowerASCII(RecalboxConf::Instance().GetEmulationstationVideoMode()));
      if (!GetResolutionFromString(resolution, w, h))
      {
        resolution = Strings::ToLowerASCII(RecalboxConf::Instance().GetGlobalVideoMode());
        GetResolutionFromString(resolution, w, h);
      }
    }
  }
  { LOG(LogInfo) << "[Renderer] Get resolution from recalbox.conf: " << w << 'x' << h; }
}

bool Renderer::ReInitialize()
{
  // Reinitialize with real resolution settings
  return Initialize(mDisplayWidth, mDisplayHeight);
}

bool Renderer::Initialize(int w, int h)
{
  GetResolutionList();

  { LOG(LogInfo) << "[Renderer] Initial resolution: " << w << 'x' << h; }

  // Get resolution from config if either w or h is nul
  bool isCrt = Board::Instance().CrtBoard().IsCrtAdapterAttached();
  bool createdSurface = false;
  if ((w * h) == 0 && !isCrt)
    GetResolutionFromConfiguration(w, h);

  if (isCrt)
  {
    // Es will choose its own resolution. The desktop mode cannot be trusted.
    if (CrtConf::Instance().GetSystemCRTResolution() == "480")
    {
      if(Board::Instance().CrtBoard().MustForce50Hz())
      {
        w = 768;
        h = 576;
      } else {
        w = 640;
        h = 480;
      }
    } else {
      if(Board::Instance().CrtBoard().MustForce50Hz())
      {
        w = 384;
        h = 288;
      } else {
        w = 320;
        h = 240;
      }
    }
  }
  if ((w * h) != 0) createdSurface = CreateSdlSurface(w, h);

  if (!createdSurface)
  {
    SDL_DisplayMode dispMode;
    SDL_GetDesktopDisplayMode(0, &dispMode);
    { LOG(LogInfo) << "[Renderer] Get resolution from desktop: " << dispMode.w << 'x' << dispMode.h; }
    createdSurface = CreateSdlSurface(dispMode.w, dispMode.h);
  }
  if (!createdSurface)
    return false;

  if(isCrt) {
    mScale = mDisplayWidthFloat / (mDisplayHeightFloat * 1.3334f);
    mVirtualDisplayWidth = (int) (mDisplayHeightFloat * 1.3334f);
    mVirtualDisplayWidthFloat = mDisplayHeightFloat * 1.3334f;
  } else {
    mScale = 1;
    mVirtualDisplayWidth = mDisplayWidth;
    mVirtualDisplayWidthFloat = mDisplayWidthFloat;
  }

  glViewport(0, 0, mDisplayWidth, mDisplayHeight);

  glMatrixMode(GL_PROJECTION);
  glOrtho(0, mDisplayWidth, mDisplayHeight, 0, -1.0, 1.0);
  glScalef(mScale,1,1);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  return true;
}

void Renderer::Finalize()
{
  DestroySdlSurface();
}

const Renderer::ResolutionList& Renderer::GetResolutionList()
{
  static ResolutionList resolutions;
  if (resolutions.empty())
  {
    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };
    int displayCount = SDL_GetNumVideoDisplays();
    { LOG(LogInfo) << "[Renderer] Number of displays: " << displayCount; }
    for (int i = 0; i < displayCount; ++i)
    {
      { LOG(LogInfo) << "[Renderer]   Display: " << i; }
      int modesCount = SDL_GetNumDisplayModes(i);
      for (int m = 0; m < modesCount; ++m)
        if (SDL_GetDisplayMode(i, m, &mode) == 0)
        {
          resolutions.push_back({ mode.w, mode.h, (int)SDL_BITSPERPIXEL(mode.format), mode.refresh_rate });
          { LOG(LogInfo) << "[Renderer]     Resolution: " << resolutions.back().ToStringAll(); }
        }
    }
  }

  return resolutions;
}

const Renderer::ResolutionList& Renderer::GetFilteredResolutionList()
{
  static ResolutionList resolutions;
  if (resolutions.empty())
  {
    for (const Resolution& resolution: GetResolutionList())
    {
      bool found = false;
      for (const Resolution& alreadyIn: resolutions)
        if ((alreadyIn.Height == resolution.Height) && (alreadyIn.Width == resolution.Width))
        {
          found = true;
          break;
        }
      if (!found && resolution.Bpp >= 15) resolutions.push_back(resolution);
    }
  }

  return resolutions;
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
  Vector4i box((int)(pos.x() * mScale), pos.y(), (int)((float)dim.x() * mScale), dim.y());
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




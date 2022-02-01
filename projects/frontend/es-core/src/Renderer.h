#pragma once

#include "platform_gl.h"
#include <stack>
#include <SDL.h>
#include <utils/math/Vector2f.h>
#include <utils/math/Vector4i.h>
#include <utils/math/Vector2i.h>
#include <utils/math/Transform4x4f.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <utils/gl/Rectangle.h>
#include <utils/gl/Vertex.h>
#include <utils/gl/Colors.h>
#include <utils/storage/Stack.h>

// Forward declaration
class Component;
class Font;

class Renderer : public StaticLifeCycleControler<Renderer>
{
  private:
    //! New clipping rectangle stack
    Stack<Rectangle> mClippings;
    //! Clipping rectangle stack
    std::stack<Vector4i> mClippingStack;

    //! SDL Surface
    SDL_Window*   mSdlWindow;
    //! SDL GL context
    SDL_GLContext mSdlGLContext;

    //! Display width as integer
    int   mDisplayWidth;
    //! Display height as integer
    int   mDisplayHeight;
    //! Display width as float
    float mDisplayWidthFloat;
    //! Display height as float
    float mDisplayHeightFloat;
    //! Virtual viewport display width as integer
    int   mVirtualDisplayWidth;
    //! Virtual viewport display width as float
    float mVirtualDisplayWidthFloat;
    //! X Scaling
    float mScale;
    //! Aspect ratio
    float mAspectRatio;

    //! True if both surface and context have been initialized
    bool mViewPortInitialized;
    //! Initial cursor state
    bool mInitialCursorState;
    //! Windowed mode
    bool mWindowed;

    static void ColorToByteArray(GLubyte* array, Colors::ColorARGB color)
    {
      array[0] = (GLubyte)(color >> 24);
      array[1] = (GLubyte)(color >> 16);
      array[2] = (GLubyte)(color >> 8);
      array[3] = (GLubyte)color;
    }

    /*!
     * @brief Create SDL display surface
     * @return True if the surface has been created successfuly
     */
    bool CreateSdlSurface(int width, int height);

    /*!
     * @brief Destroy SDL displaysurface
     */
    void DestroySdlSurface();

    /*!
     * @brief Activate GL debug callback, only in dev/DEBUG mode
     */
    static void ActivateGLDebug();

    /*!
     * @brief Get resolution from configuration file
     * @param w Width to fill
     * @param h Height to fill
     */
    static void GetResolutionFromConfiguration(int& w, int& h);

    /*!
     * @brief Get resolution from string
     * @param resolution Resolution string
     * @param w Output width
     * @param h Output height
     * @return True is a resolution has been converted, false otherwise
     */
    static bool GetResolutionFromString(const std::string& resolution, int& w, int& h);

  public:
    //! Error status
    enum class Error
    {
        NoError,        //!< Everything is fine
        OutOfMemory,    //!< Out of memory (CPU)
        OutOfGPUMemory, //!< Out of memory (GPU)
        NoResource,     //!< File not found or unknown internal resource
    };

    /*!
     * @brief Constructor
     */
    Renderer(int width, int height, bool windowed);

    /*!
     * @brief Destructor
     */
    ~Renderer();

    /*!
     * @brief Initialize GL viewport
     * @param w Required Width (0 = display width)
     * @param h Required Height (0 = display height)
     * @return true if everything is working fine, false otherwise
     */
    bool Initialize(int w, int h);

    /*!
     * @brief Reinitialize video using previous parameters
     * @return true if everything is working fine, false otherwise
     */
    bool ReInitialize();

    /*!
     * Finalize GL viewport
     */
    void Finalize();

    /*!
     * @brief Applmy the given matrix to openGL context
     * @param transform Matrix
     */
    static void SetMatrix(const Transform4x4f& transform);

    /*!
     * @brief Swap working and dipslayed buffers in double buffering context
     */
    void SwapBuffers();

    /*
     * Clipping
     */

    /*!
     * @brief Push a new clipping rectangle
     * @param pos Top/Left coordinates
     * @param dim Width/Height
     */
    void PushClippingRect(Vector2i pos, Vector2i dim);

    /*!
     * @brief Pop latest clipping rectangle
     */
    void PopClippingRect();

    /*!
     * @brief Push a new clipping rectangle
     * @param pos Top/Left coordinates
     * @param dim Width/Height
     */
    void Clip(const Rectangle& area);

    /*!
     * @brief Pop latest clipping rectangle
     */
    void Unclip();

    /*
     * Drawing
     */

    /*!
     * @brief Draw rectangle
     * @param x Left coordinate
     * @param y Top coordinate
     * @param w Width
     * @param h Height
     * @param color Color
     * @param blend_sfactor Source blending
     * @param blend_dfactor Destination blending
     */
    static void DrawRectangle(const Rectangle& area, Colors::ColorARGB color, GLenum blend_sfactor = GL_SRC_ALPHA, GLenum blend_dfactor = GL_ONE_MINUS_SRC_ALPHA);

    /*!
     * @brief Draw rectangle
     * @param x Left coordinate
     * @param y Top coordinate
     * @param w Width
     * @param h Height
     * @param color Color
     * @param blend_sfactor Source blending
     * @param blend_dfactor Destination blending
     */
    static void DrawRectangle(int x, int y, int w, int h, Colors::ColorARGB color, GLenum blend_sfactor = GL_SRC_ALPHA, GLenum blend_dfactor = GL_ONE_MINUS_SRC_ALPHA);

    /*!
     * @brief Draw rectangle
     * @param x Left coordinate
     * @param y Top coordinate
     * @param w Width
     * @param h Height
     * @param color Color
     * @param blend_sfactor Source blending
     * @param blend_dfactor Destination blending
     */
    static void DrawRectangle(float x, float y, float w, float h, Colors::ColorARGB color, GLenum blend_sfactor = GL_SRC_ALPHA, GLenum blend_dfactor = GL_ONE_MINUS_SRC_ALPHA);

    /*!
     * @brief Draw polylines
     * @param vertices Vertice list
     * @param colors Color array
     * @param count Vertex count
     */
    static void DrawLines(const Vector2f vertices[], const Colors::ColorARGB colors[], int count);

    /*!
     * @brief Draw textured triangles
     * @param id GL texture id
     * @param vertices Vertice list
     * @param colors Color list
     * @param count Vertice count
     * @param tiled draw tiled texture
     */
    static void DrawTexturedTriangles(GLuint id, const Vertex vertices[], const GLubyte colors[], int count, bool tiled);

    /*!
     * @brief Draw textured triangles using a single color
     * @param id GL texture id
     * @param vertices Vertice list
     * @param color Color
     * @param count Vertice count
     * @param tiled draw tiled texture
     */
    static void DrawTexturedTriangles(GLuint id, const Vertex vertices[], Colors::ColorARGB color, int count, bool tiled);

    /*!
     * @brief Upload Alpha texture data to GPU memory
     * @param id GL Texture id
     * @param width Data width
     * @param height Data height
     * @param data Alpha bytes (1 byte = 1 pixel)
     * @return NoError or OutOfGPUMemory
     */
    static Error UploadAlpha(GLuint id, int width, int height, const void* data);

    /*!
     * @brief Upload RGBA texture data to GPU memory
     * @param id GL Texture id
     * @param width Data width
     * @param height Data height
     * @param data RGBA bytes (4 byte = 1 pixel)
     * @return NoError or OutOfGPUMemory
     */
    static Error UploadRGBA(GLuint id, int width, int height, const void* data);

    /*!
     * @brief Upload Alpha texture data to GPU memory
     * @param id GL Texture id
     * @param width Data width
     * @param height Data height
     * @param data Alpha bytes (1 byte = 1 pixel)
     * @return NoError or OutOfGPUMemory
     */
    static Error UploadAlphaPart(GLuint id, int x, int y, int width, int height, const void* data);

    /*!
     * @brief Upload RGBA texture data to GPU memory
     * @param id GL Texture id
     * @param width Data width
     * @param height Data height
     * @param data RGBA bytes (4 byte = 1 pixel)
     * @return NoError or OutOfGPUMemory
     */
    static Error UploadRGBAPart(GLuint id, int x, int y, int width, int height, const void* data);

    /*!
     * @brief Build a GL color array
     * @param array Array pointer
     * @param color Color
     * @param count Count x Color => array
     */
    static void BuildGLColorArray(GLubyte* array, Colors::ColorARGB color, int Count);

    /*!
     * @brief Create a new texture and return its GL Identifier
     * @return GL Texture identifier
     */
    static GLuint CreateGLTexture();

    /*!
     * @brief Destroy the texture associated to the given id
     * @param id GL Texture identifier
     */
    static void DestroyGLTexture(GLuint id);

    /*
     * Accessors
     */

    //! Get display Width as integer
    int DisplayWidthAsInt() const { return mVirtualDisplayWidth; }
    //! Get display Height as integer
    int DisplayHeightAsInt() const { return mDisplayHeight; }
    //! Get display Width as float
    float DisplayWidthAsFloat() const { return mVirtualDisplayWidthFloat; }
    //! Get display Height as float
    float DisplayHeightAsFloat() const { return mDisplayHeightFloat; }

    // Is small resolution?
    bool IsSmallResolution() const { return mVirtualDisplayWidth <= 480 || mDisplayHeight <= 320; }

    //! Check if the Renderer is properly initialized
    bool Initialized() const { return mViewPortInitialized; }
};

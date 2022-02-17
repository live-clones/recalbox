#include "resources/TextureData.h"
#include "resources/ResourceManager.h"
#include "utils/Log.h"
#include "ImageIO.h"
#include <cstring>
#include "nanosvg/nanosvg.h"
#include "nanosvg/nanosvgrast.h"
#include <vector>
#include <cassert>
#include <utils/math/Misc.h>

#define DPI 96

TextureData::TextureData()
  : mTile(false),
    mTextureID(0),
    mDataRGBA(nullptr),
    mWidth(0),
    mHeight(0),
    mSourceWidth(0.0f),
    mSourceHeight(0.0f),
    mScalable(false),
    mReloadable(false),
    mSVGImage(nullptr)
{
}

TextureData::TextureData(bool tile)
  : TextureData()
{
  mTile = tile;
}

TextureData::~TextureData()
{
  reset();
}

void TextureData::reset()
{
  releaseVRAM();
  releaseRAM();
  if (mSVGImage != nullptr)
    nsvgDelete(mSVGImage);

  mWidth = mHeight = 0;
  mDataRGBA = nullptr;
  mSVGImage = nullptr;
}

void TextureData::initFromPath(const Path& path)
{
  // Just set the path. It will be loaded later
  mPath = path;
  // Only textures with paths are reloadable
  mReloadable = true;
}

bool TextureData::initSVGFromMemory(const unsigned char* fileData, size_t length)
{
  // If already initialised then don't read again
  {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mDataRGBA != nullptr)
      return true;
  }

  if (mSVGImage != nullptr)
    nsvgDelete(mSVGImage);
  mSVGImage = nullptr;

  // nsvgParse excepts a modifiable, null-terminated string
  char* copy = (char*)malloc(length + 1);
  assert(copy != nullptr);
  memcpy(copy, fileData, length);
  copy[length] = '\0';

  mSVGImage = nsvgParse(copy, "px", DPI);
  free(copy);
  if (mSVGImage == nullptr)
  {
    { LOG(LogError) << "[TextureBeta] Error parsing SVG image."; }
    return false;
  }

  // We want to rasterise this texture at a specific resolution. If the source size
  // variables are set then use them otherwise set them from the parsed file
  if ((mSourceWidth == 0.0f) && (mSourceHeight == 0.0f))
  {
    mSourceWidth = mSVGImage->width;
    mSourceHeight = mSVGImage->height;
  }
  mWidth = (size_t)Math::round(mSourceWidth);
  mHeight = (size_t)Math::round(mSourceHeight);

  if (mWidth == 0)
  {
    // auto scale width to keep aspect
    mWidth = (size_t)Math::round(((float)mHeight / mSVGImage->height) * mSVGImage->width);
  }
  else if (mHeight == 0)
  {
    // auto scale height to keep aspect
    mHeight = (size_t)Math::round(((float)mWidth / mSVGImage->width) * mSVGImage->height);
  }

  unsigned char* dataRGBA = new unsigned char[mWidth * mHeight * 4];

  NSVGrasterizer* rast = nsvgCreateRasterizer();
  float scale = Math::min((float)mHeight / mSVGImage->height, (float)mWidth / mSVGImage->width);
  nsvgRasterize(rast, mSVGImage, 0, 0, scale, dataRGBA, (int)mWidth, (int)mHeight, (int)mWidth << 2);
  nsvgDeleteRasterizer(rast);

  ImageIO::flipPixelsVert(dataRGBA, mWidth, mHeight);

  std::unique_lock<std::mutex> lock(mMutex);
  mDataRGBA = dataRGBA;

  return true;
}

bool TextureData::initImageFromMemory(const unsigned char* fileData, size_t length)
{
  size_t width = 0, height = 0;

  // If already initialised then don't read again
  {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mDataRGBA != nullptr)
      return true;
  }

  std::vector<unsigned char> imageRGBA = ImageIO::loadFromMemoryRGBA32((const unsigned char*)(fileData), length, width, height);
  if (imageRGBA.empty())
  {
    { LOG(LogError) << "[TextureBeta] Could not initialize texture from memory, invalid data!  (file path: " << mPath.ToString() << ", data ptr: " << (size_t)fileData << ", reported size: " << length << ")"; }
    return false;
  }

  mSourceWidth = width;
  mSourceHeight = height;
  mScalable = false;

  return initFromRGBA(imageRGBA.data(), width, height);
}

bool TextureData::initFromRGBA(const unsigned char* dataRGBA, size_t width, size_t height)
{
  // If already initialised then don't read again
  std::unique_lock<std::mutex> lock(mMutex);
  if (mDataRGBA != nullptr)
    return true;

  // Take a copy
  mDataRGBA = new unsigned char[width * height * 4];
  memcpy(mDataRGBA, dataRGBA, width * height * 4);
  mWidth = width;
  mHeight = height;
  return true;
}

bool TextureData::load()
{
  bool retval = false;

  // Need to load. See if there is a file
  if (!mPath.IsEmpty())
  {
    const ResourceData& data = ResourceManager::getFileData(mPath);
    // is it an SVG?
    if (mPath.Extension() == ".svg")
    {
      mScalable = true;
      retval = initSVGFromMemory((const unsigned char*)data.data(), data.size());
    }
    else
      retval = initImageFromMemory((const unsigned char*)data.data(), data.size());
  }
  return retval;
}

bool TextureData::isLoaded()
{
  std::unique_lock<std::mutex> lock(mMutex);
  return (mDataRGBA != nullptr) || (mTextureID != 0);
}

bool TextureData::uploadAndBind()
{
  // See if it's already been uploaded
  std::unique_lock<std::mutex> lock(mMutex);
  if (mTextureID != 0)
  {
    glBindTexture(GL_TEXTURE_2D, mTextureID);
  }
  else
  {
    // Load it if necessary
    if (mDataRGBA == nullptr)
    {
      return false;
    }
    // Make sure we're ready to upload
    if ((mWidth == 0) || (mHeight == 0) || (mDataRGBA == nullptr))
      return false;
    glGetError();
    //now for the openGL texture stuff
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mDataRGBA);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLint wrapMode = mTile ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
  }
  return true;
}

bool TextureData::updateFromRGBA(const unsigned char* dataRGBA, size_t width, size_t height)
{
  // First time init or reset
  if (mWidth != width || mHeight != height)
  {
    // Already exists?
    if (mWidth * mHeight != 0)
      reset();

    // First time
    if (initFromRGBA(dataRGBA, width, height))
      if (uploadAndBind()) return true;
    return false;
  }

  {
    // Update only
    std::unique_lock <std::mutex> lock(mMutex);
    if (width != mWidth) return false;
    if (height != mHeight) return false;
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataRGBA);
  }
  return true;
}

void TextureData::releaseVRAM()
{
  std::unique_lock<std::mutex> lock(mMutex);
  if (mTextureID != 0)
  {
    glDeleteTextures(1, &mTextureID);
    mTextureID = 0;
  }
}

void TextureData::releaseRAM()
{
  std::unique_lock<std::mutex> lock(mMutex);
  delete[] mDataRGBA;
  mDataRGBA = nullptr;
}

size_t TextureData::width()
{
  if (mWidth == 0)
    load();
  return mWidth;
}

size_t TextureData::height()
{
  if (mHeight == 0)
    load();
  return mHeight;
}

float TextureData::sourceWidth()
{
  if (mSourceWidth == 0)
    load();
  return mSourceWidth;
}

float TextureData::sourceHeight()
{
  if (mSourceHeight == 0)
    load();
  return mSourceHeight;
}

void TextureData::setSourceSize(float width, float height)
{
  if (mScalable)
  {
    if ((mSourceWidth != width) || (mSourceHeight != height))
    {
      mSourceWidth = width;
      mSourceHeight = height;
      releaseVRAM();
      releaseRAM();
    }
  }
}

size_t TextureData::getVRAMUsage()
{
  if ((mTextureID != 0) || (mDataRGBA != nullptr))
    return mWidth * mHeight * 4;
  else
    return 0;
}

#include <resources/Font.h>
#include <vector>
#include <Renderer.h>
#include <utils/Log.h>
#include <resources/ResourceManager.h>
#include <themes/ThemeElement.h>
#include <utils/math/Misc.h>

FT_Library Font::sLibrary = nullptr;

int Font::getSize() const
{ return mSize; }

std::map<std::pair<Path, int>, std::weak_ptr<Font> > Font::sFontMap;


// utf8 stuff
size_t Font::getNextCursor(const std::string& str, size_t cursor)
{
  // compare to character at the cursor
  const char& c = str[cursor];

  size_t result = cursor;
  if ((c & 0x80) == 0) // 0xxxxxxx, one byte character
  {
    result += 1;
  }
  else if ((c & 0xE0) == 0xC0) // 110xxxxx, two bytes left in character
  {
    result += 2;
  }
  else if ((c & 0xF0) == 0xE0) // 1110xxxx, three bytes left in character
  {
    result += 3;
  }
  else if ((c & 0xF8) == 0xF0) // 11110xxx, four bytes left in character
  {
    result += 4;
  }
  else
  {
    // error, invalid utf8 string

    // if this assert is tripped, the cursor is in the middle of a utf8 code point
    assert((c & 0xC0) != 0x80); // character is 10xxxxxx

    // if that wasn't it, something crazy happened
    assert(false);
  }

  if (str.length() < result || result < cursor) // don't go beyond the very end of the string, try and catch overflow
    return cursor;
  return result;
}

// note: will happily accept malformed utf8
size_t Font::getPrevCursor(const std::string& str, size_t cursor)
{
  if (cursor == 0)
    return 0;

  do
  {
    cursor--;
  } while (cursor > 0 && (str[cursor] & 0xC0) == 0x80); // character is 10xxxxxx

  return cursor;
}

size_t Font::moveCursor(const std::string& str, size_t cursor, int amt)
{
  if (amt > 0)
  {
    for (int i = 0; i < amt; i++)
      cursor = Font::getNextCursor(str, cursor);
  }
  else if (amt < 0)
  {
    for (int i = amt; i < 0; i++)
      cursor = Font::getPrevCursor(str, cursor);
  }

  return cursor;
}

UnicodeChar Font::readUnicodeChar(const std::string& str, size_t& cursor)
{
  const char& c = str[cursor];

  if ((c & 0x80) == 0) // 0xxxxxxx, one byte character
  {
    // 0xxxxxxx
    cursor++;
    return (UnicodeChar) c;
  }
  else if ((c & 0xE0) == 0xC0) // 110xxxxx, two bytes left in character
  {
    // 110xxxxx 10xxxxxx
    UnicodeChar val = ((str[cursor] & 0x1F) << 6) | (str[cursor + 1] & 0x3F);
    cursor += 2;
    return val;
  }
  else if ((c & 0xF0) == 0xE0) // 1110xxxx, three bytes left in character
  {
    // 1110xxxx 10xxxxxx 10xxxxxx
    UnicodeChar val = ((str[cursor] & 0x0F) << 12) | ((str[cursor + 1] & 0x3F) << 6) | (str[cursor + 2] & 0x3F);
    cursor += 3;
    return val;
  }
  else if ((c & 0xF8) == 0xF0) // 11110xxx, four bytes left in character
  {
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    UnicodeChar val = ((str[cursor] & 0x07) << 18) | ((str[cursor + 1] & 0x3F) << 12) |
                      ((str[cursor + 2] & 0x3F) << 6) | (str[cursor + 3] & 0x3F);
    cursor += 4;
    return val;
  }

  // error, invalid utf8 string

  // if this assert is tripped, the cursor is in the middle of a utf8 code point
  assert((c & 0xC0) != 0x80); // character is 10xxxxxx

  // if that wasn't it, something crazy happened
  abort();
  return 0;
}


Font::FontFace::FontFace(ResourceData&& d, int size)
  : data(d), face(nullptr)
{
  int err = FT_New_Memory_Face(sLibrary, (const unsigned char*) data.data(), (int)data.size(), 0, &face);
  (void) err;
  assert(!err);

  FT_Set_Pixel_Sizes(face, 0, size);
}

Font::FontFace::~FontFace()
{
  if (face != nullptr)
    FT_Done_Face(face);
}

void Font::initLibrary()
{
  assert(sLibrary == nullptr);
  if (FT_Init_FreeType(&sLibrary) != 0)
  {
    sLibrary = nullptr;
    { LOG(LogError) << "[Font] Error initializing FreeType!"; }
  }
}

size_t Font::getMemUsage() const
{
  size_t memUsage = 0;
  for (const auto& mTexture : mTextures)
    memUsage += (size_t)(mTexture.textureSize.x() * mTexture.textureSize.y() * 4);

  for (const auto& it : mFaceCache)
    memUsage += it.second->data.size();

  return memUsage;
}

size_t Font::getTotalMemUsage()
{
  size_t total = 0;

  auto it = sFontMap.begin();
  while (it != sFontMap.end())
  {
    if (it->second.expired())
    {
      it = sFontMap.erase(it);
      continue;
    }

    total += it->second.lock()->getMemUsage();
    it++;
  }

  return total;
}

Font::Font(int size, const Path& path)
  : mSize(size), mPath(path)
{
  assert(mSize > 0);

  mMaxGlyphHeight = 0;

  if (sLibrary == nullptr)
    initLibrary();

  // always initialize ASCII characters
  mBearingMax = 0;
  mSizeMax = 0;
  for (UnicodeChar i = 32; i < 128; i++)
  {
    Glyph* g = getGlyph(i);
    if (g->bearing.y() > mBearingMax) mBearingMax = g->bearing.y();
    float height = g->texSize.y() * (float) g->texture->textureSize.y();
    if (height > mSizeMax) mSizeMax = height;
  }

  clearFaceCache();
}

Font::~Font()
{
  unload(*ResourceManager::getInstance());
}

std::shared_ptr<Font> Font::get(int size, const Path& path)
{
  std::pair<Path, int> def(path.IsEmpty() ? getDefaultPath() : path, size);
  auto foundFont = sFontMap.find(def);
  if (foundFont != sFontMap.end())
  {
    if (!foundFont->second.expired())
      return foundFont->second.lock();
  }

  std::shared_ptr<Font> font = std::shared_ptr<Font>(new Font(def.second, def.first));
  sFontMap[def] = std::weak_ptr<Font>(font);
  ResourceManager::getInstance()->addReloadable(font);
  return font;
}

void Font::unloadTextures()
{
  for (auto& mTexture : mTextures)
  {
    mTexture.deinitTexture();
  }
}

Font::FontTexture::FontTexture()
  : textureId(0), textureSize(2048, 512), writePos(Vector2i::Zero()), rowHeight(0)
{
}

Font::FontTexture::~FontTexture()
{
  deinitTexture();
}

bool Font::FontTexture::findEmpty(const Vector2i& size, Vector2i& cursor_out)
{
  if (size.x() >= textureSize.x() || size.y() >= textureSize.y())
    return false;

  if (writePos.x() + size.x() >= textureSize.x() && writePos.y() + rowHeight + size.y() + 1 < textureSize.y())
  {
    // row full, but it should fit on the next row
    // move cursor to next row
    writePos.Set(0, writePos.y() + rowHeight + 1); // leave 1px of space between glyphs
    rowHeight = 0;
  }

  if (writePos.x() + size.x() >= textureSize.x() || writePos.y() + size.y() >= textureSize.y())
  {
    // nope, still won't fit
    return false;
  }

  cursor_out = writePos;
  writePos[0] += size.x() + 1; // leave 1px of space between glyphs

  if (size.y() > rowHeight)
    rowHeight = size.y();

  return true;
}

void Font::FontTexture::initTexture()
{
  assert(textureId == 0);

  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, textureSize.x(), textureSize.y(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
}

void Font::FontTexture::deinitTexture()
{
  if (textureId != 0)
  {
    glDeleteTextures(1, &textureId);
    textureId = 0;
  }
}

void Font::getTextureForNewGlyph(const Vector2i& glyphSize, FontTexture*& tex_out, Vector2i& cursor_out)
{
  if (!mTextures.empty())
  {
    // check if the most recent texture has space
    tex_out = &mTextures.back();

    // will this one work?
    if (tex_out->findEmpty(glyphSize, cursor_out))
      return; // yes
  }

  // current textures are full,
  // make a new one
  mTextures.push_back(FontTexture());
  tex_out = &mTextures.back();
  tex_out->initTexture();

  bool ok = tex_out->findEmpty(glyphSize, cursor_out);
  if (!ok)
  {
    { LOG(LogError) << "[Font] Glyph too big to fit on a new texture (glyph size > " << tex_out->textureSize.x() << ", " << tex_out->textureSize.y() << ")!"; }
    tex_out = nullptr;
  }
}

const std::vector<Path>& getFallbackFontPaths()
{
  static Path originalPath[] = {
    Path("/usr/share/fonts/truetype/Recalbox_icons.ttf"),
    Path("/usr/share/fonts/truetype/DroidSansFallback.ttf"),// japanese, chinese, korean
    Path("/usr/share/fonts/truetype/DejaVuSansCondensed.ttf"),
    Path("/usr/share/fonts/truetype/ubuntu_condensed.ttf"),
  };
  static std::vector<Path> fontPaths;
  if (fontPaths.empty())
  {
    for (auto& path : originalPath)
      if (ResourceManager::fileExists(path))
        fontPaths.push_back(path);
  }

  return fontPaths;
}

FT_Face Font::getFaceForChar(UnicodeChar id)
{
  const std::vector<Path>& fallbackFonts = getFallbackFontPaths();

  // look through our current font + fallback fonts to see if any have the glyph we're looking for
  for (unsigned int i = 0; i < (unsigned int) fallbackFonts.size() + 1; i++)
  {
    auto fit = mFaceCache.find(i);

    if (fit == mFaceCache.end()) // doesn't exist yet
    {
      // i == 0 -> mPath
      // otherwise, take from fallbackFonts
      const Path& path = (i == 0 ? mPath : fallbackFonts[i - 1]);
      ResourceData data = ResourceManager::getFileData(path);
      mFaceCache[i] = std::unique_ptr<FontFace>(new FontFace(std::move(data), mSize));
      fit = mFaceCache.find(i);
    }

    if (FT_Get_Char_Index(fit->second->face, id) != 0)
      return fit->second->face;
  }

  // nothing has a valid glyph - return the "real" face so we get a "missing" character
  return mFaceCache.begin()->second->face;
}

void Font::clearFaceCache()
{
  mFaceCache.clear();
}

Font::Glyph* Font::getGlyph(UnicodeChar id)
{
  // is it already loaded?
  auto it = mGlyphMap.find(id);
  if (it != mGlyphMap.end())
    return &it->second;

  // nope, need to make a glyph
  FT_Face face = getFaceForChar(id);
  if (face == nullptr)
  {
    { LOG(LogError) << "[Font] Could not find appropriate font face for character " << (int) id << " for font " << mPath.ToString(); }
    return nullptr;
  }

  FT_GlyphSlot g = face->glyph;

  if (FT_Load_Char(face, id, FT_LOAD_RENDER) != 0)
  {
    { LOG(LogError) << "[Font] Could not find glyph for character " << (int) id << " for font " << mPath.ToString() << ", size " << mSize << "!"; }
    return nullptr;
  }

  Vector2i glyphSize((int) g->bitmap.width, (int) g->bitmap.rows);

  FontTexture* tex = nullptr;
  Vector2i cursor(0);
  getTextureForNewGlyph(glyphSize, tex, cursor);

  // getTextureForNewGlyph can fail if the glyph is bigger than the max texture size (absurdly large font size)
  if (tex == nullptr)
  {
    { LOG(LogError) << "[Font] Could not create glyph for character " << (int) id << " for font " << mPath.ToString() << ", size " << mSize << " (no suitable texture found)!"; }
    return nullptr;
  }

  // create glyph
  Glyph& glyph = mGlyphMap[id];

  glyph.texture = tex;
  glyph.texPos.Set((float) cursor.x() / (float) tex->textureSize.x(),
                   (float) cursor.y() / (float) tex->textureSize.y());
  glyph.texSize.Set((float) glyphSize.x() / (float) tex->textureSize.x(),
                    (float) glyphSize.y() / (float) tex->textureSize.y());

  glyph.advance.Set((float) g->metrics.horiAdvance / 64.0f, (float) g->metrics.vertAdvance / 64.0f);
  glyph.bearing.Set((float) g->metrics.horiBearingX / 64.0f, (float) g->metrics.horiBearingY / 64.0f);

  // upload glyph bitmap to texture
  glBindTexture(GL_TEXTURE_2D, tex->textureId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, cursor.x(), cursor.y(), glyphSize.x(), glyphSize.y(), GL_ALPHA, GL_UNSIGNED_BYTE,
                  g->bitmap.buffer);
  glBindTexture(GL_TEXTURE_2D, 0);

  // update max glyph height
  if (glyphSize.y() > mMaxGlyphHeight)
    mMaxGlyphHeight = glyphSize.y();

  // done
  return &glyph;
}

// completely recreate the texture data for all textures based on mGlyphs information
void Font::rebuildTextures()
{
  // recreate OpenGL textures
  for (auto& mTexture : mTextures)
  {
    mTexture.deinitTexture();
    mTexture.initTexture();
  }

  // reupload the texture data
  for (auto& it : mGlyphMap)
  {
    FT_Face face = getFaceForChar(it.first);
    FT_GlyphSlot glyphSlot = face->glyph;

    // load the glyph bitmap through FT
    FT_Load_Char(face, it.first, FT_LOAD_RENDER);

    FontTexture* tex = it.second.texture;

    // find the position/size
    Vector2i cursor((int) (it.second.texPos.x() * (float) tex->textureSize.x()),
                    (int) (it.second.texPos.y() * (float) tex->textureSize.y()));
    Vector2i glyphSize((int) (it.second.texSize.x() * (float) tex->textureSize.x()),
                       (int) (it.second.texSize.y() * (float) tex->textureSize.y()));

    // upload to texture
    glBindTexture(GL_TEXTURE_2D, tex->textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, cursor.x(), cursor.y(), glyphSize.x(), glyphSize.y(), GL_ALPHA, GL_UNSIGNED_BYTE,
                    glyphSlot->bitmap.buffer);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::renderCharacter(unsigned int character, float x, float y, float wr, float hr, unsigned int color)
{
  // invalid character
  if (character == 0) return;

  Glyph* glyph = getGlyph(character);
  if (glyph == nullptr) return;

  struct Vertex
  {
    Vector2f pos;
    Vector2f tex;
  } vertices[6];

  FontTexture* texture = glyph->texture;

  float glyphWidth = glyph->texSize.x() * (float) glyph->texture->textureSize.x();
  float glyphHeight = glyph->texSize.y() * (float) glyph->texture->textureSize.y();

  Vector2f topLeft(0.0f + x, 0.0f + y);
  Vector2f bottomRight(glyphWidth * wr + x, glyphHeight * hr + y);

  vertices[0].pos.Set(topLeft.x(), topLeft.y());
  vertices[1].pos.Set(topLeft.x(), bottomRight.y());
  vertices[2].pos.Set(bottomRight.x(), topLeft.y());

  vertices[3].pos.Set(bottomRight.x(), topLeft.y());
  vertices[4].pos.Set(topLeft.x(), bottomRight.y());
  vertices[5].pos.Set(bottomRight.x(), bottomRight.y());

  float tx = glyph->texPos.x();
  float ty = glyph->texPos.y();
  float sx = tx + glyph->texSize.x();
  float sy = ty + glyph->texSize.y();

  vertices[0].tex.Set(tx, ty);
  vertices[1].tex.Set(tx, sy);
  vertices[2].tex.Set(sx, ty);

  vertices[3].tex.Set(sx, ty);
  vertices[4].tex.Set(tx, sy);
  vertices[5].tex.Set(sx, sy);

  GLubyte colors[6 * 4];
  Renderer::BuildGLColorArray(colors, color, 6);

  glBindTexture(GL_TEXTURE_2D, texture->textureId);
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

void Font::renderTextCache(TextCache* cache)
{
  if (cache == nullptr)
  {
    { LOG(LogError) << "[Font] Attempted to draw NULL TextCache!"; }
    return;
  }

  for (auto& vertexList : cache->vertexLists)
  {
    assert(vertexList.textureIdPtr != nullptr);

    glBindTexture(GL_TEXTURE_2D, *vertexList.textureIdPtr);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(TextCache::Vertex), vertexList.verts[0].pos.data());
    glTexCoordPointer(2, GL_FLOAT, sizeof(TextCache::Vertex), vertexList.verts[0].tex.data());
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, vertexList.colors.data());

    glDrawArrays(GL_TRIANGLES, 0, (int)vertexList.verts.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  }
}

Vector2f Font::sizeText(const std::string& text, float lineSpacing)
{
  float lineWidth = 0.0f;
  float highestWidth = 0.0f;

  const float lineHeight = getHeight(lineSpacing);

  float y = lineHeight;

  size_t i = 0;
  while (i < text.length())
  {
    UnicodeChar character = readUnicodeChar(text, i); // advances i

    if (character == (UnicodeChar) '\n')
    {
      if (lineWidth > highestWidth)
        highestWidth = lineWidth;

      lineWidth = 0.0f;
      y += lineHeight;
    }

    Glyph* glyph = getGlyph(character);
    if (glyph != nullptr)
      lineWidth += glyph->advance.x();
  }

  if (lineWidth > highestWidth)
    highestWidth = lineWidth;

  return { highestWidth, y };
}

float Font::getHeight(float lineSpacing) const
{
  return (float) mMaxGlyphHeight * lineSpacing;
}

float Font::getLetterHeight()
{
  Glyph* glyph = getGlyph((UnicodeChar) 'S');
  assert(glyph);
  return glyph->texSize.y() * (float) glyph->texture->textureSize.y();
}

// TODO: Rewrite!
//the worst algorithm ever written
//breaks up a normal string with newlines to make it fit xLen
std::string Font::wrapText(std::string text, float xLen)
{
  std::string out;

  std::string line, word, temp;

  Vector2f textSize(0);

  while (text.length() > 0) //while there's text or we still have text to render
  {
    size_t space = text.find_first_of(" \t\n");
    if (space == std::string::npos)
      space = text.length() - 1;

    word = text.substr(0, space + 1);
    text.erase(0, space + 1);

    temp = line + word;

    textSize = sizeText(temp);

    // if the word will fit on the line, add it to our line, and continue
    if (textSize.x() <= xLen)
    {
      line = temp;
      continue;
    }
    else
    {
      // the next word won't fit, so break here
      out += line + '\n';
      line = word;
    }
  }

  // whatever's left should fit
  out += line;

  return out;
}

Vector2f Font::sizeWrappedText(const std::string& text, float xLen, float lineSpacing)
{
  return sizeText(wrapText(text, xLen), lineSpacing);
}

Vector2f Font::getWrappedTextCursorOffset(const std::string& text, float xLen, size_t stop, float lineSpacing)
{
  std::string wrappedText = wrapText(text, xLen);

  float lineWidth = 0.0f;
  float y = 0.0f;

  size_t wrapCursor = 0;
  size_t cursor = 0;
  while (cursor < stop)
  {
    UnicodeChar wrappedCharacter = readUnicodeChar(wrappedText, wrapCursor);
    UnicodeChar character = readUnicodeChar(text, cursor);

    if (wrappedCharacter == (UnicodeChar) '\n' && character != (UnicodeChar) '\n')
    {
      //this is where the wordwrap inserted a newline
      //reset lineWidth and increment y, but don't consume a cursor character
      lineWidth = 0.0f;
      y += getHeight(lineSpacing);

      cursor = getPrevCursor(text, cursor); // unconsume
      continue;
    }

    if (character == (UnicodeChar) '\n')
    {
      lineWidth = 0.0f;
      y += getHeight(lineSpacing);
      continue;
    }

    Glyph* glyph = getGlyph(character);
    if (glyph != nullptr)
      lineWidth += glyph->advance.x();
  }

  return { lineWidth, y };
}

//=============================================================================================================
//TextCache
//=============================================================================================================

float Font::getNewlineStartOffset(const std::string& text, unsigned int charStart, float xLen, TextAlignment alignment)
{
  switch (alignment)
  {
    case TextAlignment::Left: return 0;
    case TextAlignment::Center:
    {
      size_t endChar = text.find('\n', charStart);
      return
        (xLen - sizeText(text.substr(charStart, endChar != std::string::npos ? endChar - charStart : endChar)).x()) /
        2.0f;
    }
    case TextAlignment::Right:
    {
      size_t endChar = text.find('\n', charStart);
      return xLen -
             (sizeText(text.substr(charStart, endChar != std::string::npos ? endChar - charStart : endChar)).x());
    }
    case TextAlignment::Top:
    case TextAlignment::Bottom:
    default: return 0;
  }
}

TextCache*
Font::buildTextCache(const std::string& text, Vector2f offset, unsigned int color, float xLen, TextAlignment alignment,
                     float lineSpacing, bool nospacing)
{
  float x = offset[0] + (xLen != 0 ? getNewlineStartOffset(text, 0, xLen, alignment) : 0);

  float yTop = mBearingMax; // getGlyph((UnicodeChar)'S')->bearing.y();
  float yBot = getHeight(lineSpacing);
  float y = offset[1] + (nospacing ? yTop : (yBot + yTop) / 2.0f);

  // vertices by texture
  std::map<FontTexture*, std::vector<TextCache::Vertex> > vertMap;

  size_t cursor = 0;
  while (cursor < text.length())
  {
    UnicodeChar character = readUnicodeChar(text, cursor); // also advances cursor

    // invalid character
    if (character == 0)
      continue;

    if (character == (UnicodeChar) '\n')
    {
      y += getHeight(lineSpacing);
      x = offset[0] +
          (xLen != 0 ? getNewlineStartOffset(text, cursor /* cursor is already advanced */, xLen, alignment) : 0);
      continue;
    }

    Glyph* glyph = getGlyph(character);
    if (glyph == nullptr)
      continue;

    std::vector<TextCache::Vertex>& verts = vertMap[glyph->texture];
    size_t oldVertSize = verts.size();
    verts.resize(oldVertSize + 6);
    TextCache::Vertex* tri = verts.data() + oldVertSize;

    const float glyphStartX = x + glyph->bearing.x();

    const Vector2i& textureSize = glyph->texture->textureSize;

    // triangle 1
    // round to fix some weird "cut off" text bugs
    tri[0].pos.Set(Math::round(glyphStartX),
                   Math::round(y + (glyph->texSize.y() * (float) textureSize.y() - glyph->bearing.y())));
    tri[1].pos.Set(Math::round(glyphStartX + glyph->texSize.x() * (float) textureSize.x()),
                   Math::round(y - glyph->bearing.y()));
    tri[2].pos.Set(tri[0].pos.x(), tri[1].pos.y());

    tri[0].tex.Set(glyph->texPos.x(), glyph->texPos.y() + glyph->texSize.y());
    tri[1].tex.Set(glyph->texPos.x() + glyph->texSize.x(), glyph->texPos.y());
    tri[2].tex.Set(tri[0].tex.x(), tri[1].tex.y());

    // triangle 2
    tri[3].pos = tri[0].pos;
    tri[4].pos = tri[1].pos;
    tri[5].pos.Set(tri[1].pos.x(), tri[0].pos.y());

    tri[3].tex = tri[0].tex;
    tri[4].tex = tri[1].tex;
    tri[5].tex.Set(tri[1].tex.x(), tri[0].tex.y());

    // advance
    x += glyph->advance.x();
  }

  //TextCache::CacheMetrics metrics = { sizeText(text, lineSpacing) };

  TextCache* cache = new TextCache();
  cache->vertexLists.resize(vertMap.size());
  cache->metrics = {sizeText(text, lineSpacing)};

  unsigned int i = 0;
  for (auto& it : vertMap)
  {
    TextCache::VertexList& vertList = cache->vertexLists[i];

    vertList.textureIdPtr = &it.first->textureId;
    vertList.verts = it.second;

    vertList.colors.resize(4 * it.second.size());
    Renderer::BuildGLColorArray(vertList.colors.data(), color, (int)it.second.size());
  }

  clearFaceCache();

  return cache;
}

TextCache*
Font::buildTextCache(const std::string& text, float offsetX, float offsetY, unsigned int color, bool nospacing)
{
  return buildTextCache(text, Vector2f(offsetX, offsetY), color, 0.0f, TextAlignment::Left, 1.5f, nospacing);
}

void TextCache::setColor(unsigned int color)
{
  for (auto& vertexList : vertexLists)
    Renderer::BuildGLColorArray(vertexList.colors.data(), color, (int)vertexList.verts.size());
}

std::shared_ptr<Font>
Font::getFromTheme(const ThemeElement* elem, ThemeProperties properties, const std::shared_ptr<Font>& orig)
{
  if (!hasFlags(properties, ThemeProperties::FontPath, ThemeProperties::FontSize))
    return orig;

  std::shared_ptr<Font> font;
  int size = (orig ? orig->mSize : (int) FONT_SIZE_MEDIUM);
  Path path = (orig ? orig->mPath : getDefaultPath());

  float sh = Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat());
  if (hasFlag(properties, ThemeProperties::FontSize) && elem->HasProperty("fontSize"))
    size = (int) (sh * elem->AsFloat("fontSize"));
  if (hasFlag(properties, ThemeProperties::FontPath) && elem->HasProperty("fontPath"))
    path = Path(elem->AsString("fontPath"));

  return get(size, path);
}

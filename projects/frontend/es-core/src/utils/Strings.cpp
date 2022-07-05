#include "utils/Strings.h"

#include <cstdarg>
#include <cstring>

#include "Unicode.h"

unsigned short Strings::sSmallToCapital[1 << (8 * sizeof(unsigned short))];
unsigned short Strings::sCapitalToSmall[1 << (8 * sizeof(unsigned short))];

bool Strings::sInitialized = Strings::sInitialize();

const std::string Strings::Empty;

bool Strings::sInitialize()
{
  memset(sSmallToCapital, 0, sizeof(sSmallToCapital));
  memset(sCapitalToSmall, 0, sizeof(sCapitalToSmall));

  for(int i = (int)(sizeof(UnicodeConverterTable) / sizeof(UnicodeConverterTable[0])); --i >= 0;)
  {
    const UnicodeConverter& c = UnicodeConverterTable[i];
    sSmallToCapital[c.Small] = c.Capital;
    sCapitalToSmall[c.Capital] = c.Small;

    // This test code ensure every single tuple encodes on the same amount of bytes
    /*std::string a = unicode2Chars((unsigned int)c.Small);
    std::string b = unicode2Chars((unsigned int)c.Capital);
    if (a.length() != b.length())
      printf("index %d not equal!", i);*/
  }

  return true;
}

std::string Strings::ToLowerUTF8(const std::string& _string)
{
  std::string result = _string; // Allocate memory once
  int l = (int)_string.length();

  for(int cursor = 0; cursor < l; )
  {
    unsigned char c = result[cursor];
    if((c & 0x80) == 0) // 0xxxxxxx, one byte character
    {
      // Simple ASCII7 uppercase
      if (c - 0x41u < 26u) result[cursor] = (char)(c | 0x20);
      cursor++;
    }
    else if((c & 0xE0) == 0xC0) // 110xxxxx, two byte character
    {
      // Unicode lowercase
      unsigned short u = sCapitalToSmall[(unsigned short)((c & 0x1F) << 6) | ((result[cursor + 1] & 0x3F))];
      if (u != 0)
      {
        result[cursor + 0] = (char)(((u >>  6) & 0xFF) | 0xC0);
        result[cursor + 1] = (char)(((u      ) & 0x3F) | 0x80);
      }
      cursor += 2;
    }
    else if((c & 0xF0) == 0xE0) // 1110xxxx, three byte character
    {
      // 1110xxxx 10xxxxxx 10xxxxxx
      unsigned short u = sCapitalToSmall[(unsigned short)((result[cursor + 0] & 0x0F) << 12) |
                                         ((result[cursor + 1] & 0x3F) <<  6) |
                                         ((result[cursor + 2] & 0x3F))];
      if (u != 0)
      {
        result[cursor + 0] = (char)(((u >> 12) & 0xFF) | 0xE0);
        result[cursor + 1] = (char)(((u >>  6) & 0x3F) | 0x80);
        result[cursor + 2] = (char)(((u      ) & 0x3F) | 0x80);
      }
      cursor += 3;
    }
    else if((c & 0xF8) == 0xF0) cursor += 4; // 11110xxx, four byte character, ignore!
    else ++cursor; // error, invalid unicode
  }

  return result;
}

std::string Strings::ToUpperUTF8(const std::string& _string)
{
  std::string result = _string; // Allocate memory once
  int l = (int)_string.length();

  for(int cursor = 0; cursor < l; )
  {
    unsigned char c = result[cursor];
    if((c & 0x80) == 0) // 0xxxxxxx, one byte character
    {
      // Simple ASCII7 lowercase
      if (c - 0x61u < 26u) result[cursor] = (char)(c & 0xDF);
      cursor++;
    }
    else if((c & 0xE0) == 0xC0) // 110xxxxx, two byte character
    {
      // Unicode lowercase
      unsigned short u = sSmallToCapital[(unsigned short)((c & 0x1F) << 6) | ((result[cursor + 1] & 0x3F))];
      if (u != 0)
      {
        result[cursor + 0] = (char)(((u >>  6) & 0xFF) | 0xC0);
        result[cursor + 1] = (char)(((u      ) & 0x3F) | 0x80);
      }
      cursor += 2;
    }
    else if((c & 0xF0) == 0xE0) // 1110xxxx, three byte character
    {
      // 1110xxxx 10xxxxxx 10xxxxxx
      unsigned short u = sSmallToCapital[(unsigned short)((result[cursor + 0] & 0x0F) << 12) |
                                         ((result[cursor + 1] & 0x3F) <<  6) |
                                         ((result[cursor + 2] & 0x3F))];
      if (u != 0)
      {
        result[cursor + 0] = (char)(((u >> 12) & 0xFF) | 0xE0);
        result[cursor + 1] = (char)(((u >>  6) & 0x3F) | 0x80);
        result[cursor + 2] = (char)(((u      ) & 0x3F) | 0x80);
      }
      cursor += 3;
    }
    else if((c & 0xF8) == 0xF0) cursor += 4; // 11110xxx, four byte character, ignore!
    else ++cursor; // error, invalid unicode
  }

  return result;
}

std::string Strings::UnicodeToUtf8(const std::vector<unsigned int>& unicodes)
{
  std::string result;
  for(unsigned int unicode : unicodes)
    result.append(unicode2Chars(unicode));
  return result;
}

std::vector<unsigned int> Strings::Utf8ToUnicode(const std::string& utf8)
{
  std::vector<unsigned int> result;
  for(int position = 0; position < (int)utf8.size(); )
    result.push_back(chars2Unicode(utf8, position));
  return result;
}

unsigned int Strings::chars2Unicode(const std::string& _string, int& _cursor)
{
	const char&  c      = _string[_cursor];
	unsigned int result = '?';

	if((c & 0x80) == 0) // 0xxxxxxx, one byte character
	{
		// 0xxxxxxx
		result = (unsigned int)((unsigned char)_string[_cursor]);
		_cursor++;
	}
	else if((c & 0xE0) == 0xC0) // 110xxxxx, two byte character
	{
		// 110xxxxx 10xxxxxx
		result = (unsigned int)((_string[_cursor] & 0x1F) <<  6) |
													 ((_string[_cursor + 1] & 0x3F)      );
		_cursor += 2;
	}
	else if((c & 0xF0) == 0xE0) // 1110xxxx, three byte character
	{
		// 1110xxxx 10xxxxxx 10xxxxxx
		result = (unsigned int)((_string[_cursor] & 0x0F) << 12) |
													 ((_string[_cursor + 1] & 0x3F) <<  6) |
													 ((_string[_cursor + 2] & 0x3F)      );
		_cursor += 3;
	}
	else if((c & 0xF8) == 0xF0) // 11110xxx, four byte character
	{
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		result = (unsigned int)((_string[_cursor] & 0x07) << 18) |
													 ((_string[_cursor + 1] & 0x3F) << 12) |
													 ((_string[_cursor + 2] & 0x3F) <<  6) |
														((_string[_cursor + 3] & 0x3F)      );
		_cursor += 4;
	}
	else
	{
		// error, invalid unicode
		++_cursor;
	}

	return result;

} // chars2Unicode

std::string Strings::unicode2Chars(const unsigned int _unicode)
{
	std::string result;

	if(_unicode < 0x80) // one byte character
	{
		result += (char)((_unicode      ) & 0xFF);
	}
	else if(_unicode < 0x800) // two byte character
	{
		result += (char)(((_unicode >>  6) & 0xFF) | 0xC0);
		result += (char)(((_unicode      ) & 0x3F) | 0x80);
	}
	else if(_unicode < 0xFFFF) // three byte character
	{
		result += (char)(((_unicode >> 12) & 0xFF) | 0xE0);
		result += (char)(((_unicode >>  6) & 0x3F) | 0x80);
		result += (char)(((_unicode      ) & 0x3F) | 0x80);
	}
	else if(_unicode <= 0x1fffff) // four byte character
	{
		result += (char)(((_unicode >> 18) & 0xFF) | 0xF0);
		result += (char)(((_unicode >> 12) & 0x3F) | 0x80);
		result += (char)(((_unicode >>  6) & 0x3F) | 0x80);
		result += (char)(((_unicode      ) & 0x3F) | 0x80);
	}
	else
	{
		// error, invalid unicode
		result += '?';
	}

	return result;

} // unicode2Chars

size_t Strings::nextCursor(const std::string& _string, const size_t _cursor)
{
	size_t result = _cursor;

	while(result < _string.length())
	{
		++result;

		if((_string[result] & 0xC0) != 0x80) // break if current character is not 10xxxxxx
			break;
	}

	return result;

} // nextCursor

size_t Strings::prevCursor(const std::string& _string, const size_t _cursor)
{
	size_t result = _cursor;

	while(result > 0)
	{
		--result;

		if((_string[result] & 0xC0) != 0x80) // break if current character is not 10xxxxxx
			break;
	}

	return result;

} // prevCursor

size_t Strings::moveCursor(const std::string& _string, const size_t _cursor, const int _amount)
{
	size_t result = _cursor;

	if(_amount > 0)
	{
		for(int i = 0; i < _amount; ++i)
			result = nextCursor(_string, result);
	}
	else if(_amount < 0)
	{
		for(int i = _amount; i < 0; ++i)
			result = prevCursor(_string, result);
	}

	return result;

} // moveCursor

std::string Strings::ToLowerASCII(const std::string& _string)
{
	std::string string = _string;

	for(int i = (int)_string.length(); --i >= 0; )
		string[i] = (char)tolower(_string[i]);

	return string;

} // ToLowerASCII

std::string Strings::ToUpperASCII(const std::string& _string)
{
	std::string string = _string;

	for(int i = (int)_string.length(); --i >= 0; )
		string[i] = (char)toupper(_string[i]);

	return string;
}

std::string Strings::Trim(const std::string& _string)
{
	const size_t strBegin = _string.find_first_not_of(" \t");
	const size_t strEnd   = _string.find_last_not_of(" \t");

	if(strBegin == std::string::npos) return std::string();

  if (strBegin != 0 || strEnd != _string.size() - 1)
  	return _string.substr(strBegin, strEnd - strBegin + 1);
  return _string;
} // Trim

std::string Strings::Trim(const std::string& _string, const char* _trimwhat)
{
  const size_t strBegin = _string.find_first_not_of(_trimwhat);
  const size_t strEnd   = _string.find_last_not_of(_trimwhat);

  if(strBegin == std::string::npos) return std::string();

  if (strBegin != 0 || strEnd != _string.size() - 1)
    return _string.substr(strBegin, strEnd - strBegin + 1);
  return _string;
} // Trim

std::string Strings::Replace(const std::string& _string, char _replace, char _with)
{
  std::string string = _string;

  for(int i = (int)string.size(); --i >= 0; )
    if (string[i] == _replace)
      string[i] = _with;
  return string;
}

std::string Strings::Replace(const std::string& _string, const std::string& _replace, const std::string& _with)
{
	std::string string = _string;

	for(int pos = 0; (pos = (int)string.find(_replace, pos)) != (int)std::string::npos; )
		string.replace(pos, _replace.length(), _with);

	return string;
}

std::string Strings::Replace(const std::string& _string, const std::string& _replace, const char* _with, int _withLength)
{
  std::string string = _string;

  for(int pos = 0; (pos = (int)string.find(_replace, pos)) != (int)std::string::npos; )
    string.replace(pos, 1, _with, _withLength);

  return string;
}

std::string& Strings::ReplaceAllIn(std::string& _string, const char _replace, const char _with)
{
  for(int i = (int)_string.size(); --i >= 0; )
    if (_string[i] == _replace)
      _string[i] = _with;
  return _string;
}

std::string& Strings::ReplaceAllIn(std::string& _string, const char _replace, const char* _with, int _withlength)
{
  for(int pos = 0; (pos = (int)_string.find(_replace, pos)) != (int)std::string::npos; pos += _withlength)
    _string.replace(pos, 1, _with, _withlength);
  return _string;
}

std::string& Strings::ReplaceAllIn(std::string& _string, const std::string& _replace, const std::string& _with)
{
  for(int pos = 0; (pos = (int)_string.find(_replace, pos)) != (int)std::string::npos; pos += (int)_with.size())
    _string.replace(pos, _replace.length(), _with);
  return _string;
}

bool Strings::StartsWith(const std::string& _string, const std::string& _start)
{
  return (strncmp(_string.c_str(), _start.c_str(), _start.size()) == 0);
}

bool Strings::StartsWith(const std::string& _string, const char* _start, int length)
{
  return (strncmp(_string.c_str(), _start, length) == 0);
}

bool Strings::EndsWith(const std::string& _string, const std::string& _end)
{
  if (_string.size() < _end.size()) return false;
	return (strncmp(_string.data() + _string.size() - _end.size(), _end.data(), _end.size()) == 0);
}

std::string Strings::RemoveParenthesis(const std::string& str)
{
	std::string s = str;

  // Remove () starting from the end to minimize char moves
  for(int end = (int)s.rfind(')'); end != (int)std::string::npos; )
  {
    int start = (int)s.rfind('(', end);
    if (start == (int)std::string::npos) break;
    while(s[++end] == ' '); // Trim right
    while(s[--start] == ' '); // Trim left
    ++start;
    s.erase(start, end - start);
    end = (int)s.rfind(')', start - 1);
  }

  // Remove [] starting from the end to minimize char moves
  for(int end = (int)s.rfind(']'); end != (int)std::string::npos; )
  {
    int start = (int)s.rfind('[', end);
    if (start == (int)std::string::npos) break;
    while(s[++end] == ' '); // Trim right
    while(s[--start] == ' '); // Trim left
    ++start;
    s.erase(start, end - start);
    end = (int)s.rfind(']', start - 1);
  }

  return s;
}

Strings::Vector Strings::Split(const std::string& _string, char splitter, int max, bool multipleSplittersAsOne)
{
  Vector vector;
  size_t start = 0;
  size_t comma = _string.find(splitter);

  while(comma != std::string::npos)
  {
    if ((int)vector.size() == max - 1) comma = _string.size();
    vector.push_back(_string.substr(start, comma - start));
    start = multipleSplittersAsOne ? _string.find_first_not_of(splitter, comma) : comma + 1;
    comma = _string.find(splitter, start);
  }
  if (start < _string.length())
    vector.push_back(_string.substr(start));

  return vector;
}

Strings::Vector Strings::Split(const std::string& _string, char splitter, bool multipleSplittersAsOne)
{
  return Split(_string, splitter, INT32_MAX, multipleSplittersAsOne);
}

std::string Strings::Join(const std::vector<std::string>& _string, const std::string& joiner)
{
  std::string result;
  for(const std::string& string : _string)
  {
    if (!result.empty()) result.append(joiner);
    result.append(string);
  }
  return result;
}

std::string Strings::Join(const std::vector<const char*>& _string, const std::string& joiner)
{
  std::string result;
  for(const std::string& string : _string)
  {
    if (!result.empty()) result.append(joiner);
    result.append(string);
  }
  return result;
}

std::string Strings::Join(const std::vector<std::string>& _string, char joiner)
{
  std::string result;
  for(const std::string& string : _string)
  {
    if (!result.empty()) result.append(1, joiner);
    result.append(string);
  }
  return result;
}

std::string Strings::Join(const std::vector<const char*>& _string, char joiner)
{
  std::string result;
  for(const std::string& string : _string)
  {
    if (!result.empty()) result.append(1, joiner);
    result.append(string);
  }
  return result;
}

std::string Strings::Format(const char* _string, ...)
{
	va_list	args;
	va_list copy;

	va_start(args, _string);

	va_copy(copy, args);
	const int length = vsnprintf(nullptr, 0, _string, copy);
	va_end(copy);

	std::string result;
	result.resize(length);
	va_copy(copy, args);
	vsnprintf((char*)result.c_str(), (size_t)length + 1, _string, copy);
	va_end(copy);

	va_end(args);

	return result;
} // format

// Simple XOR scrambling of a string, with an accompanying key
std::string Strings::ScrambleSymetric(const std::string& _input, const std::string& key)
{
	std::string buffer = _input;

	for (size_t i = 0; i < _input.size(); ++i)
	{
		buffer[i] = (char)(_input[i] ^ (key[i] + (i*17)));
	}

	return buffer;
} // scramble

std::string Strings::ScrambleSymetric2(const std::string& _input, const std::string& key)
{
  std::string buffer = _input;
  int l = (int)key.size();

  for (size_t i = 0; i < _input.size(); ++i)
  {
    buffer[i] = (char)(_input[i] ^ (key[i % l] + (i*17)));
  }

  return buffer;
} // scramble

#define INT32BUFFERLEN 12
#define INT64BUFFERLEN 22
#define FLOATBUFFERLEN 40

bool Strings::ToInt(const std::string& source, int index, char stop, int& out)
{
  if (index >= (int)source.size()) return false;
  const char* src = source.c_str() + index;

  bool Sign = (src[0] == '-');
  if (Sign) src++;

  int Result = 0;
  while ((unsigned int)(src[0] - 0x30) <= 9) { Result *= 10; Result += src[0] - 0x30; src++; }
  if (src[0] != stop) return false;

  out = Sign ? -Result : Result;
  return true;
}

bool Strings::ToLong(const std::string& source, int index, char stop, long long int& out)
{
  if (index >= (int)source.size()) return false;
  const char* src = source.c_str() + index;

  bool Sign = (src[0] == '-');
  if (Sign) src++;

  long long Result = 0;
  while ((unsigned int)(src[0] - 0x30) <= 9) { Result *= 10; Result += src[0] - 0x30; src++; }
  if (src[0] != stop) return false;

  out = Sign ? -Result : Result;
  return true;
}

bool Strings::ToFloat(const std::string& source, int index, char stop, float& out)
{
  if (index >= (int)source.size()) return false;
  const char* src = source.c_str() + index;

  bool Sign = (src[0] == '-');
  if (Sign) src++;

  long long IntPart = 0;
  long long FracPart = 0;
  long long Pow10 = 1;

  // Integer part
  while ((unsigned int)(src[0] - 0x30) <= 9) { IntPart *= 10; IntPart += src[0] - 0x30; src++; }
  if (src[0] == '.')
  {
    src++;
    while ((unsigned int)(src[0] - 0x30) <= 9) { FracPart *= 10; FracPart += src[0] - 0x30; src++; Pow10 *= 10; }
  }
  if (src[0] != stop) return false;

  float Result = (float)IntPart + ((float)FracPart / (float)Pow10);

  out = Sign ? -Result : Result;
  return true;
}

bool Strings::ToBool(const std::string& source, int index, char stop, bool& out)
{
  if (index >= (int)source.size()) return false;
  const char* src = source.c_str() + index;

  // Try numeric
  if ((src[0] == '1') && (src[1] == stop)) { out = true; return true; }
  if ((src[0] == '0') && (src[1] == stop)) { out = false; return true; }

  // Try textual
  const int Mask = 0xDF;
  if (((src[0] & Mask) == 'T') && ((src[1] & Mask) == 'R') && ((src[2] & Mask) == 'U') && ((src[3] & Mask) == 'E') && (src[4] == stop)) { out = true; return true; }
  if (((src[0] & Mask) == 'F') && ((src[1] & Mask) == 'A') && ((src[2] & Mask) == 'L') && ((src[3] & Mask) == 'S') && ((src[4] & Mask) == 'E') && (src[5] == stop)) { out = false; return true; }

  return false;
}

std::string Strings::ToString(int integer)
{
  char Buffer[INT32BUFFERLEN]; Buffer[INT32BUFFERLEN - 1] = 0;
  int Index = INT32BUFFERLEN - 1;
  bool Sign = (integer < 0);
  if (Sign) integer = -integer;

  do { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; } while (integer != 0);
  if (Sign) Buffer[--Index] = '-';

  return std::string(Buffer + Index, (INT32BUFFERLEN - 1) - Index);
}

std::string Strings::ToString(unsigned int integer)
{
  char Buffer[INT32BUFFERLEN]; Buffer[INT32BUFFERLEN - 1] = 0;
  int Index = INT32BUFFERLEN - 1;

  do { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; } while (integer != 0);

  return std::string(Buffer + Index, (INT32BUFFERLEN - 1) - Index);
}

std::string Strings::ToString(long long integer)
{
  char Buffer[INT64BUFFERLEN]; Buffer[INT64BUFFERLEN - 1] = 0;
  int Index = INT64BUFFERLEN - 1;
  bool Sign = (integer < 0);
  if (Sign) integer = -integer;

  do { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; } while (integer != 0);
  if (Sign) Buffer[--Index] = '-';

  return std::string(Buffer + Index, (INT64BUFFERLEN - 1) - Index);
}

std::string Strings::ToString(unsigned long long integer)
{
  char Buffer[INT64BUFFERLEN]; Buffer[INT64BUFFERLEN - 1] = 0;
  int Index = INT64BUFFERLEN - 1;

  do { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; } while (integer != 0);

  return std::string(Buffer + Index, (INT64BUFFERLEN - 1) - Index);
}

static float Pow10[] =
{
  1.0f,
  10.0f,
  100.0f,
  1000.0f,
  10000.0f,
  100000.0f,
  1000000.0f,
  10000000.0f,
  100000000.0f,
  1000000000.0f,
  10000000000.0f,
};

std::string Strings::ToString(float value, int precision)
{
  // Sign
  bool sign = value < 0.0f;
  if (sign) value = -value;

  // Extract integer part
  int integer = (int)value;
  // Extract floating part
  float fpart = value - (float)integer;

  char Buffer[FLOATBUFFERLEN * 2]; Buffer[FLOATBUFFERLEN - 1] = 0;
  int Index = FLOATBUFFERLEN - 1;

  do { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; } while (integer != 0);
  if (sign) Buffer[--Index] = '-';

  // check for display option after point
  if (precision > 0)
  {
    int Start = Index;
    Buffer[FLOATBUFFERLEN - 1] = '.';
    if (precision > 10) precision = 10;

    Index = FLOATBUFFERLEN + precision; Buffer[Index] = 0;
    fpart *= Pow10[precision]; integer = (int)fpart;
    while(Index > FLOATBUFFERLEN) { Buffer[--Index] = (char)(0x30 + (integer % 10)); integer /= 10; }

    return std::string(Buffer + Start, (FLOATBUFFERLEN - Start) + precision);
  }

  return std::string(Buffer + Index, (FLOATBUFFERLEN - 1) - Index);
}

bool Strings::HexToInt(const std::string& from, int index, char stop, int& out)
{
  if (index >= (int)from.size()) return false;
  const char* src = from.c_str() + index;

  if (src[0] != 0)
  {
    if (src[0] == '$') src++;
    else if (src[1] != 0)
      if ((src[0] == '0') && (src[1] == 'x')) src += 2;
  }

  int Result = 0;
  for (;; src++)
  {
    int v = (unsigned char)src[0];
    if ((unsigned int)(v - 0x30) <= 9) { Result <<= 4; Result += v - 0x30; }
    else
    {
      v &= 0xDF;
      if ((unsigned int)(v - 0x41) <= 5) { Result <<= 4; Result += v - 0x37; }
      else break;
    }
  }
  if (src[0] != stop) return false;

  out = Result;
  return true;
}

std::string Strings::ToString(bool value)
{
  return value ? "1" : "0";
}

int Strings::ToHash(const std::string& string)
{
  int count = (int)string.size();
  unsigned int Hash = (unsigned int)count;
  const unsigned char* p = (unsigned char*)string.c_str();
  while(--count >= 0) { Hash = ((Hash >> 27) | (Hash << 5)) ^ p[0]; p++; }
  return (int)Hash;
}

int Strings::ToHash(const char* string)
{
  int count = (int)strlen(string);
  unsigned int Hash = (unsigned int)count;
  const unsigned char* p = (unsigned char*)string;
  while(--count >= 0) { Hash = ((Hash >> 27) | (Hash << 5)) ^ p[0]; p++; }
  return (int)Hash;
}

unsigned long long Strings::ToHash64(const std::string& string)
{
  int count = (int)string.size();
  unsigned long long Hash = (unsigned long long)count;
  const unsigned char* p = (unsigned char*)string.c_str();
  while(--count >= 0) { Hash = ((Hash >> 59) | (Hash << 5)) ^ p[0]; p++; }
  return Hash;
}

unsigned long long Strings::ToHash64(const char* string)
{
  int count = (int)strlen(string);
  unsigned long long Hash = (unsigned long long)count;
  const unsigned char* p = (unsigned char*)string;
  while(--count >= 0) { Hash = ((Hash >> 59) | (Hash << 5)) ^ p[0]; p++; }
  return Hash;
}

static const char* HexaChars = "0123456789ABCDEF";

std::string Strings::ToHexa(int hex)
{
  char Buffer[10];
  int Index = sizeof(Buffer);
  do { Buffer[--Index] = HexaChars[hex & 0xF]; hex = (int)((unsigned int)hex >> 4); } while(hex != 0);
  return std::string(Buffer + Index, sizeof(Buffer) - Index);
}

std::string Strings::ToHexa(long long hex)
{
  char Buffer[18];
  int Index = sizeof(Buffer);
  do { Buffer[--Index] = HexaChars[hex & 0xF]; hex = (long long)((unsigned long long)hex >> 4); } while(hex != 0);
  return std::string(Buffer + Index, sizeof(Buffer) - Index);
}

std::string Strings::ToHexa(int hex, int length)
{
  char Buffer[10];
  if ((unsigned int)length > 8) length = 8;
  int Index = sizeof(Buffer);
  do { Buffer[--Index] = HexaChars[hex & 0xF]; hex = (int)((unsigned int)hex >> 4); } while(--length != 0);
  return std::string(Buffer + Index, sizeof(Buffer) - Index);
}

std::string Strings::ToHexa(long long hex, int length)
{
  char Buffer[18];
  if ((unsigned int)length > 16) length = 16;
  int Index = sizeof(Buffer);
  do { Buffer[--Index] = HexaChars[hex & 0xF]; hex = (long long)((unsigned long long)hex >> 4); } while(--length != 0);
  return std::string(Buffer + Index, sizeof(Buffer) - Index);
}

std::string Strings::URLEncode(const std::string& source)
{
  std::string result;
  const char* p = source.c_str();

  for (int i = (int)source.length(); --i >= 0;)
  {
    unsigned char C = (unsigned char)*p++;
    if (((C >= 'a') && (C <= 'z')) ||
        ((C >= 'A') && (C <= 'Z')) ||
        ((C >= '0') && (C <= '9')) ||
        (C == '_') ||
        (C == '*') ||
        (C == '.') ||
        (C == '-')) result.append(1, (char)C);
    else
    {
      char buffer[3] = { '%', HexaChars[C >> 4], HexaChars[C & 0xF] };
      result.append(buffer, 3);
    }
  }

  return result;
}

int Strings::CountChar(const std::string& source, char c)
{
  int count = 0;
  const char* p = source.c_str();
  for (int i = (int)source.length(); --i >= 0;)
    if (p[i] == c) count++;
  return count;
}

std::string Strings::Extract(const std::string& source, const char* starttag, const char* endtag, int starttagl, int endtagl)
{
  int start = (int)source.find(starttag, 0, starttagl);
  if (start != (int)std::string::npos)
  {
    int stop = (int)source.find(endtag, start + starttagl, endtagl);
    if (stop != (int)std::string::npos)
      return source.substr(start + starttagl, stop - (start + starttagl));
  }
  return std::string();
}

bool Strings::Contains(const std::string& source, const char* what)
{
  return (source.find(what) != std::string::npos);
}

bool Strings::Contains(const std::string& source, const std::string& what)
{
  return (source.find(what) != std::string::npos);
}

bool Strings::Contains(const char* source, const char* what)
{
  return (strstr(source, what) != nullptr);
}

bool Strings::SplitAt(const std::string& _string, char splitter, std::string& left, std::string& right, bool trim)
{
  size_t pos = _string.find(splitter);
  if (pos == std::string::npos) return false;

  left  = _string.substr(0, pos);
  right = _string.substr(pos + 1);
  if (trim)
  {
    left = Trim(left, " \t\r\n");
    right = Trim(right, " \t\r\n");
  }

  return true;
}

Strings::Vector Strings::SplitQuotted(const std::string& _string, char splitter)
{
  return SplitQuotted(_string, splitter, INT32_MAX);
}

Strings::Vector Strings::SplitQuotted(const std::string& _string, char splitter, int max)
{
  int Len = (int)_string.size();
  const char* Current = _string.data();
  const char* Last = Current;
  const char* End = Current + Len;
  bool Quoted = false;

  Strings::Vector output;
  if (max > 1)
    for (; --Len >= 0; ++Current)
    {
      char C = Current[0];
      if (C == '"') Quoted = !Quoted;
      else if ((C == splitter) && !Quoted)
      {
        if (Last < Current) output.push_back(std::string(Last, (int)(Current - Last)));
        Last = Current + 1;
        if ((int)output.size() == max - 1) break;
      }
    }
  output.push_back(std::string(Last, (int)(End - Last)));

  return output;
}

std::string Strings::ToHumanSize(long long int size)
{
  // Bytes
  if (size < (1 << 10)) return ToString((int)size).append(1, 'B');
  // KB
  if (size < (1 << 20)) return ToString((float)size / 1024.f, 2).append("KB", 2);
  // MB
  size >>= 10;
  if (size < (1 << 20)) return ToString((float)size / 1024.f, 2).append("MB", 2);
  // GB
  size >>= 10;
  if (size < (1 << 20)) return ToString((float)size / 1024.f, 2).append("GB", 2);
  // TB
  size >>= 10;
  return ToString((float)size / 1024.f, 2).append("TB", 2);
}

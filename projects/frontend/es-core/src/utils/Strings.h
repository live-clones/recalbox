#pragma once
#ifndef ES_CORE_UTILS_STRING_UTIL_H
#define ES_CORE_UTILS_STRING_UTIL_H

#include <string>
#include <vector>

class Strings
{
  private:
    static unsigned short _SmallToCapital[1 << (8 * sizeof(unsigned short))];
    static unsigned short _CapitalToSmall[1 << (8 * sizeof(unsigned short))];

    static bool _Initialized;

    static bool _Initialize();

  public:
    typedef std::vector<std::string> Vector;

    static unsigned int chars2Unicode(const std::string& _string, size_t& _cursor);

    static std::string unicode2Chars(unsigned int _unicode);

    static size_t nextCursor(const std::string& _string, size_t _cursor);

    static size_t prevCursor(const std::string& _string, size_t _cursor);

    static size_t moveCursor(const std::string& _string, size_t _cursor, int _amount);

    static std::string ToLowerUTF8(const std::string& _string);

    static std::string ToUpperUTF8(const std::string& _string);

    static std::string ToLowerASCII(const std::string& _string);

    static std::string ToUpperASCII(const std::string& _string);

    static std::string Trim(const std::string& _string);

    static std::string Trim(const std::string& _string, const char* _trimwhat);

    static std::string Replace(const std::string& _string, const std::string& _replace, const std::string& _with);

    static bool StartsWith(const std::string& _string, const std::string& _start);

    static bool EndsWith(const std::string& _string, const std::string& _end);

    static std::string RemoveParenthesis(const std::string& _string);

    static Vector Split(const std::string& _string, char splitter);

    static std::string Join(const std::vector<std::string>& _string, const std::string& joiner);

    static std::string Format(const char* _string, ...);

    static std::string ScrambleSymetric(const std::string& _input, const std::string& key);

    static std::string ScrambleSymetric2(const std::string& _input, const std::string& key);

    static bool ToInt(const std::string& source, int index, char stop, int& out);

    static bool ToInt(const std::string& source, int index, int& out) { return ToInt(source, index, 0, out); }

    static bool ToInt(const std::string& source, int& out) { return ToInt(source, 0, 0, out); }

    static bool ToLong(const std::string& source, int index, char stop, long int& out);

    static bool ToLong(const std::string& source, int index, long int& out) { return ToLong(source, index, 0, out); }

    static bool ToLong(const std::string& source, long int& out) { return ToLong(source, 0, 0, out); }

    static bool ToFloat(const std::string& source, int index, char stop, float& out);

    static bool ToFloat(const std::string& source, int index, float& out) { return ToFloat(source, index, 0, out); }

    static bool ToFloat(const std::string& source, float& out) { return ToFloat(source, 0, 0, out); }

    static bool ToBool(const std::string& source, int index, char stop, bool& out);

    static bool ToBool(const std::string& source, int index, bool& out) { return ToBool(source, index, 0, out); }

    static bool ToBool(const std::string& source, bool& out) { return ToBool(source, 0, 0, out); }

    static bool HexToInt(const std::string& from, int index, char stop, int& out);

    static bool HexToInt(const std::string& from, int index, int& out) { return HexToInt(from, index, 0, out); }

    static bool HexToInt(const std::string& from, int& out) { return HexToInt(from, 0, 0, out); }

    static std::string ToString(bool value);

    static std::string ToString(int value);

    static std::string ToString(unsigned int value);

    static std::string ToString(long long value);

    static std::string ToString(unsigned long long value);

    static std::string ToString(float value, int precision);

    static int ToHash(const std::string& string);

    static int ToHash(const char* string);
}; // String::

#endif // ES_CORE_UTILS_STRING_UTIL_H

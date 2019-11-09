#pragma once
#ifndef ES_CORE_UTILS_STRING_UTIL_H
#define ES_CORE_UTILS_STRING_UTIL_H

#include <string>
#include <vector>

class StringUtil
{
  private:
    static unsigned short _SmallToCapital[1 << (8 * sizeof(unsigned short))];
    static unsigned short _CapitalToSmall[1 << (8 * sizeof(unsigned short))];

    static bool _Initialized;

    static bool _Initialize();

  public:
    typedef std::vector<std::string> stringVector;

    static unsigned int chars2Unicode(const std::string& _string, size_t& _cursor);

    static std::string unicode2Chars(unsigned int _unicode);

    static size_t nextCursor(const std::string& _string, size_t _cursor);

    static size_t prevCursor(const std::string& _string, size_t _cursor);

    static size_t moveCursor(const std::string& _string, size_t _cursor, int _amount);

    static std::string toLowerUTF8(const std::string& _string);

    static std::string toUpperUTF8(const std::string& _string);

    static std::string toLower(const std::string& _string);

    static std::string toUpper(const std::string& _string);

    static std::string trim(const std::string& _string);

    static std::string trim(const std::string& _string, const char* _trimwhat);

    static std::string replace(const std::string& _string, const std::string& _replace, const std::string& _with);

    static bool startsWith(const std::string& _string, const std::string& _start);

    static bool endsWith(const std::string& _string, const std::string& _end);

    static std::string removeParenthesis(const std::string& _string);

    static stringVector splitString(const std::string& _string, char splitter);

    static std::string joinStrings(const std::vector<std::string>& _string, const std::string& joiner);

    static stringVector commaStringToVector(const std::string& _string);

    static std::string vectorToCommaString(stringVector _vector);

    static std::string format(const char* _string, ...);

    static std::string scramble(const std::string& _input, const std::string& key);

    static bool TryToInt(const std::string& source, int index, char stop, int& out);

    static bool TryToInt(const std::string& source, int index, int& out) { return TryToInt(source, index, 0, out); }

    static bool TryToInt(const std::string& source, int& out) { return TryToInt(source, 0, 0, out); }

    static bool TryToLong(const std::string& source, int index, char stop, long int& out);

    static bool TryToLong(const std::string& source, int index, long int& out) { return TryToLong(source, index, 0, out); }

    static bool TryToLong(const std::string& source, long int& out) { return TryToLong(source, 0, 0, out); }

    static bool TryToFloat(const std::string& source, int index, char stop, float& out);

    static bool TryToFloat(const std::string& source, int index, float& out) { return TryToFloat(source, index, 0, out); }

    static bool TryToFloat(const std::string& source, float& out) { return TryToFloat(source, 0, 0, out); }

    static bool TryToBool(const std::string& source, int index, char stop, bool& out);

    static bool TryToBool(const std::string& source, int index, bool& out) { return TryToBool(source, index, 0, out); }

    static bool TryToBool(const std::string& source, bool& out) { return TryToBool(source, 0, 0, out); }

    static  std::string ToString(int integer);

    static std::string ToString(unsigned int integer);

    static std::string ToString(long long integer);

    static std::string ToString(unsigned long long integer);

    static std::string ToString(float value, int precision);
}; // String::

#endif // ES_CORE_UTILS_STRING_UTIL_H

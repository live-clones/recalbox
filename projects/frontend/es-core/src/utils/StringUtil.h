#pragma once
#ifndef ES_CORE_UTILS_STRING_UTIL_H
#define ES_CORE_UTILS_STRING_UTIL_H

#include <string>
#include <vector>

class StringUtil
{
	public:
		typedef std::vector<std::string> stringVector;

		static unsigned int chars2Unicode      (const std::string& _string, size_t& _cursor);
		static std::string  unicode2Chars      (const unsigned int _unicode);
		static size_t       nextCursor         (const std::string& _string, const size_t _cursor);
		static size_t       prevCursor         (const std::string& _string, const size_t _cursor);
		static size_t       moveCursor         (const std::string& _string, const size_t _cursor, const int _amount);
		static std::string  toLower            (const std::string& _string);
		static std::string  toUpper            (const std::string& _string);
		static std::string  trim               (const std::string& _string);
		static std::string  replace            (const std::string& _string, const std::string& _replace, const std::string& _with);
		static bool         startsWith         (const std::string& _string, const std::string& _start);
		static bool         endsWith           (const std::string& _string, const std::string& _end);
		static std::string  removeParenthesis  (const std::string& _string);
		static stringVector commaStringToVector(const std::string& _string);
		static std::string  vectorToCommaString(stringVector _vector);
		static std::string  format             (const char* _string, ...);
		static std::string  scramble           (const std::string& _input, const std::string& key);
}; // String::

#endif // ES_CORE_UTILS_STRING_UTIL_H

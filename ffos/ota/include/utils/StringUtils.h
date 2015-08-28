/*
 * StringUtils.h
 *
 *  Created on: 2014-7-12
 *      Author: manson
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <string>

namespace utils {

class StringUtils {
public:
	StringUtils();
	virtual ~StringUtils();

	static std::string GetFileNameFromUrl(const std::string &url);

	static bool StartWith(const std::string &str, const std::string &prefix);

	static int FindFirstSymbol(const std::string &symbol,
			const std::string &str);
	static void ShiftStringLeft(std::string &str, int length);
	static void ShiftStringRight(std::string &str, int length);
	static void ShiftString(std::string &str, int start, int length);
	static std::string HeadSubString(std::string &str, int length);

	static bool IsUTF8(const void* pBuffer, long size);
	static int IsTextUTF8(const char* str, long length);
};

}

#endif /* STRINGUTILS_H_ */

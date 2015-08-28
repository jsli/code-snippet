/*
 * StringUtils.cpp
 *
 *  Created on: 2014-7-12
 *      Author: manson
 */

#include "utils/StringUtils.h"

namespace utils {

StringUtils::StringUtils() {
	// TODO Auto-generated constructor stub
}

StringUtils::~StringUtils() {
	// TODO Auto-generated destructor stub
}

std::string StringUtils::GetFileNameFromUrl(const std::string &url) {
	std::string trimed = url.substr(url.find_last_of('/') + 1, url.size());
	return trimed;
}

bool StringUtils::StringUtils::StartWith(const std::string &str,
		const std::string &prefix) {
	return (str.compare(0, prefix.size(), prefix) == 0) ? true : false;
}

int StringUtils::FindFirstSymbol(const std::string &symbol,
		const std::string &str) {
	return str.find_first_of(symbol);
}

void StringUtils::ShiftStringLeft(std::string &str, int length) {
	ShiftString(str, 0, length);
}

void StringUtils::ShiftStringRight(std::string &str, int length) {
	int start = str.size() - length;
	ShiftString(str, start, length);
}

void StringUtils::ShiftString(std::string &str, int start, int length) {
	str.erase(start, length);
}

std::string StringUtils::HeadSubString(std::string &str, int length) {
	return str.substr(0, length);
}

bool StringUtils::IsUTF8(const void* pBuffer, long size) {
	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*) pBuffer;
	unsigned char* end = (unsigned char*) pBuffer + size;
	while (start < end) {
		if (*start < 0x80) // (10000000): ��煎��浜�0x80���涓�ASCII瀛�绗�
				{
			start++;
		} else if (*start < (0xC0)) // (11000000): ��间��浜�0x80涓�0xC0涔���寸��涓烘�����UTF-8瀛�绗�
				{
			IsUTF8 = false;
			break;
		} else if (*start < (0xE0)) // (11100000): 姝よ����村��涓�2瀛����UTF-8瀛�绗�
				{
			if (start >= end - 1)
				break;
			if ((start[1] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 2;
		} else if (*start < (0xF0)) // (11110000): 姝よ����村��涓�3瀛����UTF-8瀛�绗�
				{
			if (start >= end - 2)
				break;
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 3;
		} else {
			IsUTF8 = false;
			break;
		}
	}
	return IsUTF8;
}

int StringUtils::IsTextUTF8(const char* str, long length) {
	int i;
	int nBytes = 0; //UFT8������1-6涓�瀛����缂����,ASCII��ㄤ��涓�瀛����
	unsigned char chr;
	bool bAllAscii = true; //濡������ㄩ�ㄩ�芥��ASCII, 璇存��涓����UTF-8
	for (i = 0; i < length; i++) {
		chr = *(str + i);
		if ((chr & 0x80) != 0) { // ��ゆ��������ASCII缂����,濡����涓����,璇存����������芥��UTF-8,ASCII���7浣�缂����,浣���ㄤ��涓�瀛����瀛�,���楂�浣����璁颁负0,o0xxxxxxx
			bAllAscii = false;
		}
		if (nBytes == 0) { //濡����涓����ASCII���,搴�璇ユ��澶�瀛����绗�,璁＄��瀛�������
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else {
					return false;
				}
				nBytes--;
			}
		} else { //澶�瀛����绗�������棣�瀛����,搴�涓� 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			nBytes--;
		}
	}

	if (nBytes > 0) { //杩�杩�瑙����
		return false;
	}

	if (bAllAscii) { //濡������ㄩ�ㄩ�芥��ASCII, 璇存��涓����UTF-8
		return false;
	}
	return true;
}

}

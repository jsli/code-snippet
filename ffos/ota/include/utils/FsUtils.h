/*
 * fs_utils.h
 *
 *  Created on: 2014-5-20
 *      Author: manson
 */

#ifndef FS_UTILS_H_
#define FS_UTILS_H_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace utils {

class FsUtils {
public:
	FsUtils();
	virtual ~FsUtils();

	static bool IsDirExist(const std::string &path);
	static bool IsFileExist(const std::string &path);
	static bool MkDir(const std::string &path);
	static std::string ReadFile(const std::string &path);
	static bool WriteFile(const std::string &path, const std::string &content);
	static void DeleteFile(const std::string &path);
	static off_t GetFileSize(const std::string &path);
	static int CleanDir(const std::string &path);
};

}

#endif /* FS_UTILS_H_ */

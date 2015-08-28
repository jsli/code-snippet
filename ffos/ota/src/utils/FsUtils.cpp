/*
 * fs_utils.cpp
 *
 *  Created on: 2014-5-20
 *      Author: manson
 */

#include "utils/FsUtils.h"

namespace utils {

FsUtils::FsUtils() {
	// TODO Auto-generated constructor stub
}

FsUtils::~FsUtils() {
	// TODO Auto-generated destructor stub
}

bool FsUtils::IsDirExist(const std::string &path) {
	DIR *dir = opendir(path.c_str());
	if (dir == NULL) {
		return false;
	}
	return true;
}

bool FsUtils::IsFileExist(const std::string &path) {
	return access(strdup(path.c_str()), 0) == 0;
}

bool FsUtils::MkDir(const std::string &path) {
	if (IsDirExist(path)) {
		return true;
	}
	int res = mkdir(path.c_str(), 0755);
	if (res == 0) {
		return true;
	}
	return false;
}

std::string FsUtils::ReadFile(const std::string &path) {
	std::ifstream in;
	in.open(path.c_str(), std::ios::in);
	std::string line;
	std::stringstream ss;
	while (!in.eof()) {
		getline(in, line);
		ss << line << std::endl;
	}
	in.close();
	return ss.str();
}

bool FsUtils::WriteFile(const std::string &path, const std::string &content) {
	std::ofstream outfile(path.c_str(), std::ios::out);
	if (!outfile || !outfile.is_open()) {
		return false;
	}
	outfile << content;
	outfile.flush();
	outfile.close();
	return true;
}

void FsUtils::DeleteFile(const std::string &path) {
	remove(strdup(path.c_str()));
}

off_t FsUtils::GetFileSize(const std::string &path) {
	struct stat info;
	stat(path.c_str(), &info);
	return info.st_size;
}

int FsUtils::CleanDir(const std::string &path) {
	std::stringstream ss;
	ss << "rm -r -f ";
	ss << path;
	ss << "*";
	std::string p = ss.str();
	return system(p.c_str());
}

}

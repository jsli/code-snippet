#include <string>
#include <stdlib.h>

#include "download/DownloadTask.h"
#include "utils/Logger.h"
#include "utils/FsUtils.h"
#include "utils/StringUtils.h"
#include "json/json.h"
#include "ota/config/OtaConfig.h"

namespace download {

DownloadTask::DownloadTask() {
}

DownloadTask::~DownloadTask() {
}

DownloadTask * DownloadTask::CreateNewTask(const std::string &metadata) {
	DownloadTask * task = new DownloadTask();
	if (!task->load(metadata)) {
		delete task;
		return NULL;
	}
	return task;
}

DownloadTask * DownloadTask::CreateTaskFromCache() {
	DownloadTask * task = new DownloadTask();
	if (!task->loadFromCache()) {
		delete task;
		return NULL;
	}
	return task;
}

DownloadTask * DownloadTask::CreateFakeTask() {
	DownloadTask * task = new DownloadTask();
	task->m_Url =
			"http://yinyueshiting.baidu.com/data2/music/121949522/12152327672000128.mp3?xcode=ce67ce2106763a96bc707c7a06af5b211185dbd0cbb5163e";
	task->m_DownloadPath = "/Users/manson/temp/"
			+ utils::StringUtils::GetFileNameFromUrl(task->m_Url);
	task->m_FileSize = 4851304.0;
	task->m_Md5 = "badc5ef7336f7f8f1477e00841422e54";
	return task;
}

bool DownloadTask::load(const std::string &metadata) {
	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(metadata, value)) {
		Log("download task parse failed : %s", metadata.c_str());
		return false;
	} else {
		m_Url = value["url"].asString();
		std::string fileName = utils::StringUtils::GetFileNameFromUrl(GetUrl());
		m_DownloadPath = CACHE_ROOT + fileName;
		m_Md5 = value["md5"].asString();
		m_FileSize = value["size"].asDouble();
		m_Progress = 0;
		m_Breakpoint = 0;
	}
	return true;
}

bool DownloadTask::loadFromCache() {
	Log("load from cache : %s", DOWNLOAD_TASK_CACHE_FILE.c_str());
	if (!utils::FsUtils::IsFileExist(DOWNLOAD_TASK_CACHE_FILE)) {
		Log("load from cache : %s not exist", DOWNLOAD_TASK_CACHE_FILE.c_str());
		return false;
	}
	std::string task = utils::FsUtils::ReadFile(DOWNLOAD_TASK_CACHE_FILE);
	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(task, value)) {
		Log("download task parse failed : %s", task.c_str());
		return false;
	} else {
		m_Url = value["url"].asString();
		std::string fileName = utils::StringUtils::GetFileNameFromUrl(GetUrl());
		m_DownloadPath = CACHE_ROOT + fileName;
		m_Md5 = value["md5"].asString();
		m_FileSize = value["file_size"].asDouble();
		m_Progress = value["progress"].asDouble();
		m_Breakpoint = value["breakpoint"].asDouble();
	}
	return true;
}

std::string DownloadTask::dump() {
	Json::Value root;

	root["url"] = GetUrl();
	root["download_path"] = GetDownloadPath();
	root["md5"] = GetMd5();
	root["file_size"] = Json::Value(GetFileSize());
	root["progress"] = Json::Value(GetProgress());
	root["breakpoint"] = Json::Value(GetBreakpoint());

	return root.toString();
}

void DownloadTask::CacheSelf() {
	utils::FsUtils::WriteFile(DOWNLOAD_TASK_CACHE_FILE, dump());
}

void DownloadTask::LogSelf() {
	Log("%s", dump().c_str());
}

std::string DownloadTask::GetUrl() {
	return m_Url;
}

std::string DownloadTask::GetMd5() {
	return m_Md5;
}

std::string DownloadTask::GetDownloadPath() {
	return m_DownloadPath;
}

double DownloadTask::GetFileSize() {
	return m_FileSize;
}

double DownloadTask::GetProgress() {
	return m_Progress;
}

void DownloadTask::SetProgress(double progress) {
	m_Progress = progress;
}

double DownloadTask::GetBreakpoint() {
	return m_Breakpoint;
}

void DownloadTask::SetBreakpoint(double progress) {
	m_Breakpoint = progress;
}

}

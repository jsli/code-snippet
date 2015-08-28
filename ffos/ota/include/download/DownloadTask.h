#ifndef _DOWNLOAD_TASK_H_
#define _DOWNLOAD_TASK_H_

#include <string>

namespace download {

class DownloadTask {
private:
	std::string m_DownloadPath;
	std::string m_Url;
	std::string m_Md5;
	double m_FileSize;
	double m_Progress;
	double m_Breakpoint;

	DownloadTask();
	bool loadFromCache();
	bool load(const std::string &metadata);
	std::string dump();

public:
	static DownloadTask * CreateFakeTask();
	static DownloadTask * CreateTaskFromCache();
	static DownloadTask * CreateNewTask(const std::string &metadata);

	virtual ~DownloadTask();

	void CacheSelf();

	std::string GetDownloadPath();
	std::string GetUrl();
	std::string GetMd5();
	double GetFileSize();

	double GetProgress();
	void SetProgress(double progress);

	double GetBreakpoint();
	void SetBreakpoint(double progress);

	void LogSelf();
};

}

#endif

/*
 * CheckDaemon.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include <unistd.h>
#include <sys/types.h>

#include "ota/daemon/CheckDaemon.h"
#include "ota/config/OtaConfig.h"
#include "utils/Logger.h"
#include "utils/FsUtils.h"
#include "net/HttpClient.h"
#include "json/json.h"

namespace ota {

class DownloadListener: public download::Downloader::OnDownloadListener,
		public event::EventTarget {
private:
	int counter;
public:
	DownloadListener() :
			OnDownloadListener(), EventTarget() {
		counter = 0;
	}
	~DownloadListener() {
	}

	void onRetry(download::Downloader *downloader, int count) {
		Log("listener: download retry");
		Json::Value message;
		message["message"] = "retry";
		send(message.toString());
	}

	void onStart(download::Downloader *downloader) {
		Log("listener: download started");
		Json::Value message;
		message["message"] = "start";
		send(message.toString());
	}

	void onFailed(download::Downloader *downloader, const std::string &reason) {
		Log("listener: download failed");
		Json::Value message;
		message["message"] = "failed";
		send(message.toString());
	}

	void onSuccess(download::Downloader *downloader) {
		Log("listener: download success");
		Json::Value message;
		message["message"] = "success";
		message["path"] = downloader->GetDownloadTask()->GetDownloadPath();
		send(message.toString());
	}

	void onProgress(download::Downloader *downloader, double total,
			double progress) {
		counter++;
		if ((counter % 500) == 0 || (progress == total)) {
			Log(
					"CALLBACK: download progress: total=%f, progress=%f, percent=%%%f",
					total, progress, (progress * 100) / total);
		}
	}

	void send(const std::string message) {
		SendEvent(DOWNLOAD_EVENT, message);
	}

	std::string GetName() {
		return "DownloadListener";
	}
};

CheckDaemon::CheckDaemon() :
		Thread(true) {
	// TODO Auto-generated constructor stub
	m_DownloadListener = new DownloadListener();
	m_OnDownloadListener = m_DownloadListener;
	m_EventTarget = m_DownloadListener;
}

CheckDaemon::~CheckDaemon() {
	// TODO Auto-generated destructor stub
	delete m_DownloadListener;
}

HttpResult * CheckDaemon::check() {
	std::string params = HttpParameter::GetInstance()->GetParameters();
	Log("Http request paramter: %s", params.c_str());
	net::HttpClient *hc = new net::HttpClient();
	std::string response = hc->PostRequest(OTA_SERVER_API, params);
	HttpResult * result = new HttpResult();
	if (!result->Load(response)) {
		delete result;
		result = NULL;
	}
	return result;
}

download::DownloadTask * CheckDaemon::createDownloadTask(HttpResult * result) {
	download::DownloadTask * task = NULL;
	download::DownloadTask * cacheTask =
			download::DownloadTask::CreateTaskFromCache();
	do {
		if (cacheTask == NULL) {
			Log("no cache task");
			break;
		}
		if (cacheTask->GetUrl() != result->GetUrl()) {
			Log("cache url ->[%s]", cacheTask->GetUrl().c_str());
			Log("result url ->[%s]", result->GetUrl().c_str());
			Log("url not match");
			break;
		}
		if (cacheTask->GetFileSize() != result->GetFileSize()) {
			Log("cache file size ->[%f]", cacheTask->GetFileSize());
			Log("result file size ->[%f]", result->GetFileSize());
			Log("file size not match");
			break;
		}
		if (!utils::FsUtils::IsFileExist(cacheTask->GetDownloadPath())) {
			Log("cache file missed ->[%s]",
					cacheTask->GetDownloadPath().c_str());
			break;
		}

		double downloadedSize = utils::FsUtils::GetFileSize(
				cacheTask->GetDownloadPath());
		double progress = cacheTask->GetProgress();
		if (downloadedSize != progress) {
			Log("downloaded size ->[%f]", downloadedSize);
			Log("cache progress size ->[%f]", progress);
			Log("progress not match");
			break;
		}

		task = cacheTask;
	} while (false);

	if (task == NULL) {
		if (cacheTask != NULL) {
			delete cacheTask;
		}
		utils::FsUtils::CleanDir(CACHE_ROOT);
		Log("create new DownloadTask");
		task = download::DownloadTask::CreateNewTask(result->ToString());
	}
	return task;
}

void CheckDaemon::Run() throw (utils::Exception) {
	Log("CheckDaemon Start...[%d]", gettid());

	//wait for network connection
	sleep(30);

	int counter = 0;
	while (true) {
		Log("CheckDaemon run <%d> times", counter++);
		HttpResult * result = NULL;
		download::DownloadTask * task = NULL;
		download::Downloader * downloader = NULL; //must not delete downloader
		do {
			result = check();
			if (result == NULL) {
				break;
			}
			result->LogSelf();

			task = createDownloadTask(result);
			if (task == NULL) {
				break;
			}
			task->LogSelf();

			downloader = new download::Downloader(task);
			downloader->SetDownloadListener(m_OnDownloadListener);
			if (downloader->Start() != 0) {
				break;
			}

			Log("Downloader running, wait...");
			if (downloader->Join() != 0) {
				break;
			}
			Log("Downloader finished!");
		} while (false);

		delete result;
		delete task;

		sleep(CHECK_INTERVAL);
	}
}

event::EventTarget * CheckDaemon::GetEventTarget() {
	return m_EventTarget;
}

}

#ifndef _DOWNLOADER_H_
#define _DOWNLOADER_H_

#include <string>
#include <pthread.h>

#include "DownloadTask.h"
#include "thread/Thread.h"
#include "utils/Exception.h"

namespace download {

class Downloader: public thread::Thread {
public:
	class OnDownloadListener {
	public:
		OnDownloadListener() {
		}
		virtual ~OnDownloadListener() {
		}

		virtual void onStart(Downloader *downloader) {
		}
		virtual void onRetry(Downloader *downloader, int counter) {
		}
		virtual void onFailed(Downloader *downloader,
				const std::string &reason) {
		}
		virtual void onSuccess(Downloader *downloader) {
		}
		virtual void onProgress(Downloader *downloader, double total,
				double progress) {
		}
	};

private:
	DownloadTask * m_Task;
	OnDownloadListener * m_Listener;

protected:
	virtual void Run() throw (utils::Exception);
	bool checkDownloadTask();
	double requireFileSize();
	bool download();

public:
	Downloader(DownloadTask * task);
	virtual ~Downloader();

	DownloadTask * GetDownloadTask();
	void SetDownloadListener(OnDownloadListener *callback);
	OnDownloadListener * GetDownloadListener();

	bool IsSuccess();
};

}

#endif

#include <errno.h>
#include <sys/types.h>

#include "download/Downloader.h"
#include "utils/FsUtils.h"
#include "utils/Md5.h"
#include "net/curl/curl.h"

namespace download {

const int DOWNLOAD_RETRY_TIMES = 10;
const int DOWNLOAD_RETRY_INTERVAL = 5;
const int CONNECTION_TIMEOUT = 60;

size_t saveToFile(char *buffer, size_t size, size_t nmemb, void *userdata);
int onDownloadProgress(void* p, double dtotal, double dnow, double ultotal,
		double ulnow);

Downloader::Downloader(DownloadTask * task) :
		Thread(true) {
	m_Task = task;
	m_Listener = NULL;
}

Downloader::~Downloader() {
}

DownloadTask* Downloader::GetDownloadTask() {
	return m_Task;
}

void Downloader::SetDownloadListener(OnDownloadListener *callback) {
	m_Listener = callback;
}

Downloader::OnDownloadListener * Downloader::GetDownloadListener() {
	return m_Listener;
}

void Downloader::Run() throw (utils::Exception) {
	Log("Downloader Start...[%d]", gettid());
	if (m_Task == NULL) {
		throw utils::Exception("DownloadTask didn't set");
	}
	if (m_Listener == NULL) {
		throw utils::Exception("OnDownloadListener didn't set");
	}

	if (!checkDownloadTask()) {
		m_Listener->onFailed(this, "checkDownloadTask() failed.");
		return;
	}

	int retryCounter = 0;
	do {
		if (download()) {
			//ensure file is complete
			sync();
			sync();
			sync();
			if (utils::MD5::Md5Checksum(m_Task->GetDownloadPath(),
					m_Task->GetMd5())) {
				m_Listener->onSuccess(this);
				return;
			} else {
				Log("download finished, bug md5sum not match. retry");
			}
		}
		retryCounter++;
		m_Listener->onRetry(this, retryCounter);
		sleep(retryCounter * DOWNLOAD_RETRY_INTERVAL);
	} while (retryCounter <= DOWNLOAD_RETRY_TIMES);

	m_Listener->onFailed(this, "download failed!");
}

bool Downloader::checkDownloadTask() {
	int retryCounter = 0;
	do {
		double size = requireFileSize();
		if (size > 0) {
			if (m_Task->GetFileSize() == size) {
				return true;
			} else {
				Log("real size is %f, task size is %f", size,
						m_Task->GetFileSize());
				return false;
			}
		}

		retryCounter++;
		sleep(retryCounter * DOWNLOAD_RETRY_INTERVAL);
	} while (retryCounter < DOWNLOAD_RETRY_TIMES);
	return false;
}

double Downloader::requireFileSize() {
	CURL *curl;
	char errbuf[CURL_ERROR_SIZE] = { 0 };
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, m_Task->GetUrl().c_str());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONNECTION_TIMEOUT);
		CURLcode ret = curl_easy_perform(curl);

		long responseCode = 0;
		double size = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
		curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
		curl_easy_cleanup(curl);
		if (CURLE_OK == ret && responseCode == 200) {
			return size;
		} else {
			Log("err info = %s", errbuf);
			Log("response code = %ld", responseCode);
			return -1;
		}
	}
	return -1;
}

bool Downloader::download() {
	CURL *curl;
	FILE *fp = NULL;
	char errbuf[CURL_ERROR_SIZE] = { 0 };
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, m_Task->GetUrl().c_str());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		double breakpoint = m_Task->GetProgress();
		if (breakpoint == m_Task->GetFileSize()) { //already download finished
			return true;
		}
		m_Task->SetBreakpoint(breakpoint);
		if (breakpoint > 0) {
			fp = fopen(m_Task->GetDownloadPath().c_str(), "a");
			curl_easy_setopt(curl, CURLOPT_RESUME_FROM, (long )breakpoint);
		} else {
			fp = fopen(m_Task->GetDownloadPath().c_str(), "w");
		}

		if (fp == NULL) {
			Log("cannot open file [%s].", m_Task->GetDownloadPath().c_str());
			return false;
		}

		m_Listener->onStart(this);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * ) fp);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, saveToFile);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, onDownloadProgress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONNECTION_TIMEOUT);
		CURLcode ret = curl_easy_perform(curl);

		long responseCode = 0;
		double size;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
		curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
		curl_easy_cleanup(curl);
		fclose(fp);

		if (CURLE_OK == ret && (responseCode == 200 || responseCode == 206)
				&& (m_Task->GetFileSize() == (size + breakpoint))) {
			Log("download success");
			return true;
		} else {
			Log("err info = %s", errbuf);
			Log("response code = %ld", responseCode);
			return false;
		}
	}
	return false;
}

int onDownloadProgress(void* p, double total, double now, double ultotal,
		double ulnow) {
	if (p == NULL) {
		return -1;
	}
	Downloader *downloader = (Downloader *) p;
	if (downloader != NULL) {
		double breakPoint = downloader->GetDownloadTask()->GetBreakpoint();
		downloader->GetDownloadTask()->SetProgress(breakPoint + now);
		downloader->GetDownloadTask()->CacheSelf();
		downloader->GetDownloadListener()->onProgress(downloader, total, now);
	}
	return 0;
}

size_t saveToFile(char *buffer, size_t size, size_t nmemb, void *userdata) {
	FILE *fp = (FILE *) userdata;
	size_t return_size = fwrite(buffer, size, nmemb, fp);
	fflush(fp);
	return return_size;
}

}

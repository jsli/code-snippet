/*
 * CheckDaemon.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef CHECKDAEMON_H_
#define CHECKDAEMON_H_

#include "thread/Thread.h"
#include "ota/net/HttpParameter.h"
#include "ota/net/HttpResult.h"
#include "download/DownloadTask.h"
#include "download/Downloader.h"
#include "event/EventTarget.h"

namespace ota {

class DownloadListener;

class CheckDaemon: public thread::Thread {
private:
	event::EventTarget * m_EventTarget;
	download::Downloader::OnDownloadListener * m_OnDownloadListener;
	DownloadListener * m_DownloadListener;

	HttpResult * check();
	download::DownloadTask * createDownloadTask(HttpResult * result);

protected:
	virtual void Run() throw (utils::Exception);

public:
	CheckDaemon();
	virtual ~CheckDaemon();

	event::EventTarget * GetEventTarget();
};

}

#endif /* CHECKDAEMON_H_ */

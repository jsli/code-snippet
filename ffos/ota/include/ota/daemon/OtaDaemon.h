/*
 * OtaDaemon.h
 *
 *  Created on: 2014-8-21
 *      Author: manson
 */

#ifndef OTADAEMON_H_
#define OTADAEMON_H_

#include <string>

#include "event/EventTarget.h"
#include "utils/Exception.h"
#include "ota/daemon/CheckDaemon.h"
#include "ota/daemon/ServerDaemonImpl.h"
#include "socket/ServerDaemon.h"

namespace ota {

class OtaDaemon: public event::EventTarget {
private:
	CheckDaemon * m_CheckDaemon;
	ServerDaemonImpl * m_ServerDaemonImpl;
	sock::ServerDaemon * m_ServerDaemon;
	EventTarget * m_EventTarget;

	bool init();
	void checkLastUpdate();
	void sendNotification(const std::string &notification);

protected:
	void onEvent(const std::string &type, const std::string &message);
	std::string GetName();

public:
	OtaDaemon();
	virtual ~OtaDaemon();

	void Start() throw (utils::Exception);
};

}

#endif /* OTADAEMON_H_ */

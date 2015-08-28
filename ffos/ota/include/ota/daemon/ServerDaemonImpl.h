/*
 * ServerDaemonImpl.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef SERVERDAEMONIMPL_H_
#define SERVERDAEMONIMPL_H_

#include "socket/ServerDaemon.h"
#include "event/EventTarget.h"
#include "utils/MutexQueue.h"
#include "ota/message/IMessage.h"

namespace ota {

class ServerDaemonImpl: public sock::ServerDaemon, public event::EventTarget {
private:
	bool m_HasUpdateResult;
	bool m_UpdateResult;
	bool m_FirstConnection;
	sock::ClientSocket * m_HomeConnection;
	utils::MutexQueue<IMessage> * m_MessageQueue;

protected:
	void onMessageReceived(sock::ClientSocket *client,
			const std::string &message);
	void onNewConnection(sock::ClientSocket *conn);
	void onCloseConnection(sock::ClientSocket *conn);
	void onReadError(sock::ClientSocket *conn);
	void flushMessage();

	void onEvent(const std::string &type, const std::string &message);
	std::string GetName();

public:
	ServerDaemonImpl(int port);
	virtual ~ServerDaemonImpl();
};

}

#endif /* SERVERDAEMONIMPL_H_ */

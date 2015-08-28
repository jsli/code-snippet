/*
 * ServerDaemon.h
 *
 *  Created on: 2014-8-21
 *      Author: manson
 */

#ifndef SERVERDAEMON_H_
#define SERVERDAEMON_H_

#include <map>

#include "thread/Thread.h"
#include "socket/ServerSocket.h"
#include "utils/Exception.h"

namespace sock {

class ServerDaemon: public thread::Thread {
private:
	ClientSocket* findSessionByFd(int fd);
	void removeSession(ClientSocket *client);
	void cleanSessions(bool force);
	void addSession(ClientSocket *client);

protected:
	ServerSocket * m_Server;
	std::map<int, ClientSocket*> *m_Sessions; //<clientFd, ClientSocket>

	virtual void Run() throw (utils::Exception);
	virtual void onMessageReceived(ClientSocket *client,
			const std::string &message) = 0;
	virtual void onNewConnection(ClientSocket *conn) = 0;
	virtual void onCloseConnection(ClientSocket *conn) = 0;
	virtual void onReadError(ClientSocket *conn) = 0;
	virtual void flushMessage() = 0;

public:
	ServerDaemon(int port);
	virtual ~ServerDaemon();
};

}

#endif /* SERVERDAEMON_H_ */

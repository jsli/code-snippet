/*
 * ServerDaemon.cpp
 *
 *  Created on: 2014-8-21
 *      Author: manson
 */

#include <string>
#include <unistd.h>
#include <queue>
#include <sstream>
#include <sys/types.h>

#include "socket/ServerDaemon.h"
#include "socket/ServerSocket.h"
#include "socket/ClientSocket.h"

namespace sock {

ServerDaemon::ServerDaemon(int port) :
		Thread(true) {
	// TODO Auto-generated constructor stub
	m_Server = new ServerSocket(port);
	m_Sessions = new std::map<int, ClientSocket*>();
}

ServerDaemon::~ServerDaemon() {
	// TODO Auto-generated destructor stub
	delete m_Server;

	cleanSessions(true);
	delete m_Sessions;
}

void ServerDaemon::Run() throw (utils::Exception) {
	int res = m_Server->Start();
	if (res < 0) {
		throw utils::Exception("ServerSocket start failed.");
	}

	Log("ServerDaemon Start...[%d]", gettid());

	while (true) {
		int serverFd = m_Server->GetSocketFd();
		fd_set read_set;
		FD_ZERO(&read_set);
		FD_SET(serverFd, &read_set);

		int maxFd = serverFd;
		std::map<int, ClientSocket*>::iterator it = m_Sessions->begin();
		while (it != m_Sessions->end()) {
			int fd = it->first;
			FD_SET(fd, &read_set);
			maxFd = (fd > maxFd) ? fd : maxFd;
			it++;
		}

		//select time out, 10 milliseconds
		struct timeval tv;
		tv.tv_sec = 10 / 1000;
		tv.tv_usec = (10 % 1000) * 1000;

		if (select(maxFd + 1, &read_set, NULL, NULL, &tv) > 0) {
			//TODO: optimize acceptance event handling
			if (FD_ISSET(serverFd, &read_set)) { //check socket server
				ClientSocket* session = m_Server->AcceptConnection();
				if (session != NULL) {
					int cliFd = session->GetSocketFd();
					if (cliFd > 0) {
						if (!findSessionByFd(cliFd)) {
							addSession(session);
							onNewConnection(session);
						}
					} else if (cliFd == 0) {
						//do nothing???
					} else {
						Log("accept error, ABORT!!!");
						throw utils::Exception(
								"accept error, abort ServerDaemon.");
					}
				}
			}

			//check each session
			std::map<int, ClientSocket*>::iterator it = m_Sessions->begin();
			while (it != m_Sessions->end()) {
				int fd = it->first;
				ClientSocket *session = it->second;
				if (FD_ISSET(fd, &read_set)) {
					//read data
					std::queue<std::string> msgQueue;
					int res = session->ReadData(msgQueue);
					if (res < 0) {
						onReadError(session);
					} else {
						while (msgQueue.size() > 0) {
							std::string msg = msgQueue.front();
							onMessageReceived(session, msg);
							msgQueue.pop();
						}
					}
				}
				it++;
			}

			//remove closed sessions
			Log("before clean sessions, size = %d", m_Sessions->size());
			cleanSessions(false);
			Log("select end, session size = %d", m_Sessions->size());
		}
		flushMessage();
	}
}

void ServerDaemon::addSession(ClientSocket *client) {
	m_Sessions->insert(
			std::pair<int, ClientSocket*>(client->GetSocketFd(), client));
}

ClientSocket* ServerDaemon::findSessionByFd(int fd) {
	if (fd <= 0) {
		return NULL;
	}

	std::map<int, ClientSocket*>::iterator it = m_Sessions->begin();
	while (it != m_Sessions->end()) {
		if (fd == it->first) {
			return it->second ? it->second : NULL;
		}
		it++;
	}
	return NULL;
}

void ServerDaemon::cleanSessions(bool force) {
	std::map<int, ClientSocket*>::iterator it = m_Sessions->begin();
	while (it != m_Sessions->end()) {
		ClientSocket *session = it->second;
		std::map<int, ClientSocket*>::iterator tmpIt = it;
		it++;
		if (force || !session->IsConnected()) {
			m_Sessions->erase(tmpIt);
			onCloseConnection(session);
			session->Close();
			delete session;
		}
	}
}

void ServerDaemon::removeSession(ClientSocket *session) {
	if (!session) {
		return;
	}

	std::map<int, ClientSocket*>::iterator it = m_Sessions->find(
			session->GetSocketFd());
	if (it != m_Sessions->end()) {
		m_Sessions->erase(it);
		onCloseConnection(session);
		session->Close();
		delete session;
	}
}

}

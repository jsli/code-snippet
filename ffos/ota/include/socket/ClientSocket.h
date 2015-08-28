/*
 * ClientSocket.h
 *
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <string>
#include <queue>

#include "utils/Exception.h"

namespace sock {

const std::string MESSAGE_SEPARATOR = ":";

class ClientSocket {
private:
	int m_Fd;
	std::string m_ReadBuffer;

	static std::string TAG;

public:
	ClientSocket(int id = 0);
	virtual ~ClientSocket();

	/*
	 * create a client socket and connect to the special address
	 * return values:
	 *  < 0 : system error
	 *  = 0 : the socket already exist
	 *  > 0 : the client socket id, success
	 */
	int Connect(const std::string &addr, int port, bool block = false)
			throw (utils::Exception);

	int GetSocketFd() const;
	bool IsConnected() const;
	void Close();

	int ReadData(std::queue<std::string> &q) throw (utils::Exception);
	int WriteData(const std::string &data) throw (utils::Exception);
};

}

#endif /* CLIENTSOCKET_H_ */

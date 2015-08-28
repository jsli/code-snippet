#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "socket/ClientSocket.h"
#include "utils/Exception.h"

namespace sock {

const unsigned int DEFAULT_BUFFER_SIZE = 1024;

class ServerSocket {
private:
	int m_Fd;
	int m_Port;
	bool m_Started;

	static std::string TAG;

protected:
	int init();

public:
	ServerSocket(int port);
	virtual ~ServerSocket();

	int GetSocketFd() throw (utils::Exception);
	int GetPort() throw (utils::Exception);

	int Start() throw (utils::Exception);
	int Stop();

	/**
	 * Accept the client socket connection, must not called in main thread
	 * return values:
	 *  < 0 : the server socket has some issus, need recreate
	 *  = 0 : non client socket connecting
	 *  > 0 : return the client socket id, success
	 */
	ClientSocket * AcceptConnection() throw (utils::Exception);
};

}

#endif /* SERVERSOCKET_H_ */

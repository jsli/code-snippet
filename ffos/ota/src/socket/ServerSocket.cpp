#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include "socket/ServerSocket.h"
#include "utils/Logger.h"

namespace sock {

ServerSocket::ServerSocket(int port) {
	m_Fd = 0;
	m_Port = port;
	m_Started = false;
}

ServerSocket::~ServerSocket() {
	if (m_Started) {
		throw utils::Exception("must call Stop() first");
	}
}

int ServerSocket::GetSocketFd() throw (utils::Exception) {
	if (!m_Started) {
		throw utils::Exception("must call Start() first");
	}
	return m_Fd;
}

int ServerSocket::GetPort() throw (utils::Exception) {
	if (!m_Started) {
		throw utils::Exception("must call Start() first");
	}
	return m_Port;
}

ClientSocket * ServerSocket::AcceptConnection() throw (utils::Exception) {
	if (!m_Started) {
		throw utils::Exception("must call Start() first");
	}

	struct sockaddr_storage cliAddr; // Client address
	socklen_t cliAddrLen = sizeof(cliAddr);

	Log("AcceptConnection.....");
	int cliFd = accept(m_Fd, (struct sockaddr *) &cliAddr, &cliAddrLen);
	Log("accept client = %d", cliFd);

	//------manson-----//
	//usually, a non-negative value returned by accept means success.
	//but fd 0 is the fd of stdin, we should not use stdin in socket.
	//so we trade 0 as an unavailable fd.

	//-----keep original design------//
	if (cliFd < 0) {
		Log("accept error - clientFd=%d, "
				"error=%d:%s", cliFd, errno, strerror(errno));
		return NULL;
	} else if (cliFd == 0) {
		return NULL;
	}

	ClientSocket *client = new ClientSocket(cliFd);
	return client;
}

int ServerSocket::Start() throw (utils::Exception) {
	if (m_Started) {
		throw utils::Exception("must call Start() only once");
	}

	Log("ServerSocket Start...");
	int fd = init();
	if (fd <= 0) {
		Log("init socket error, abort");
		return -1;
	}
	m_Fd = fd;
	m_Started = true;
	return 0;
}

int ServerSocket::Stop() {
	m_Started = false;
	if (m_Fd > 0) {
		close(m_Fd);
		m_Fd = -1;
		return 0;
	}
	return -1;
}

int ServerSocket::init() {
	Log("bind on port:%d", m_Port);
	char strPort[10] = { 0 };
	sprintf(strPort, "%d", m_Port);

	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_STREAM;
	addrCriteria.ai_protocol = IPPROTO_TCP;

	// List of server addresses
	struct addrinfo *servAddr;
	int rtnVal = getaddrinfo(NULL, strPort, &addrCriteria, &servAddr);
	if (rtnVal != 0 || servAddr == NULL) {
		Log("getaddrinfo failed! servAddr=%p, error=%d:%s", servAddr, errno,
				strerror(errno));
		return -1;
	}

	struct addrinfo *addr = 0;
	int fd = 0;
	int err = 0;
	for (addr = servAddr; addr != 0; addr = addr->ai_next) {
		int reuse_on = 1;
		int nodelay = 1;
		int keep_alive = 1;
		fd = socket(servAddr->ai_family, servAddr->ai_socktype,
				servAddr->ai_protocol);
		if (fd < 0) {
			Log("create failed, errno = %d:%s", errno, strerror(errno));
			err = errno;
			continue;
		} else if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_on,
				sizeof(reuse_on))) < 0) {
			Log("reuse %d failed, errer = %d:%s", fd, errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay,
				sizeof(nodelay))) < 0) {
			Log("nodelay %d failed, errer = %d:%s", fd, errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
				(void*) &keep_alive, sizeof(keep_alive))) != 0) {
			Log("keepalive %d failed, errer = %d: %s", fd, errno,
					strerror(errno));
			err = errno;
		} else if (bind(fd, servAddr->ai_addr, servAddr->ai_addrlen)) {
			err = errno;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(servAddr->ai_family, servAddr, addressBuffer,
					INET6_ADDRSTRLEN);
			Log("bind %d failed, addr=%s, error=%d:%s", fd, addressBuffer, err,
					strerror(err));
		} else if (listen(fd, SOMAXCONN)) {
			err = errno;
			Log("listen %d failed, errno=%d:%s", fd, errno, strerror(errno));
		} else {
			int flags = fcntl(fd, F_GETFL, 0);
			fcntl(fd, F_SETFL, flags | O_NONBLOCK);
			err = 0;
			break;
		}
		close(fd);
		fd = 0;
	}
	freeaddrinfo(servAddr);

	if (err > 0 && fd <= 0) {
		Log("init failed, errno = %d:%s", err, (char *) strerror(err));
		errno = err;
		return -1;
	} else if (err == 0 && fd == 0) {
		Log("init failed without error");
		return 0;
	}

	Log("init success, sockFd = %d", fd);
	return fd;
}

}

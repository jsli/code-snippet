#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <fcntl.h>

#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "socket/ClientSocket.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"

namespace sock {

const int BUFFER_SIZE = 4096;

ClientSocket::ClientSocket(int id) {
	m_Fd = id;
}

ClientSocket::~ClientSocket() {
	if (IsConnected()) {
		throw utils::Exception("must call Close() first!");
	}
}

int ClientSocket::Connect(const std::string &addr, int port, bool block)
		throw (utils::Exception) {
	if (IsConnected()) {
		Log("Socket is already connected : %d", m_Fd);
		throw utils::Exception("Socket is already connected");
	}

	Log("connect to %s:%d", addr.c_str(), port);
	char strPort[10] = { 0 };
	sprintf(strPort, "%d", port);

	// Tell the system what kind(s) of address info we want
	struct addrinfo addrCriteria; // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC; // v4 or v6 is OK
	addrCriteria.ai_socktype = SOCK_STREAM; // Only streaming sockets
	addrCriteria.ai_protocol = IPPROTO_TCP; // Only TCP protocol

	// Get address(es)
	struct addrinfo *servAddr; // Holder for returned list of server addrs
	const char* pAddr = addr.size() > 0 ? addr.data() : NULL;
	int rtnVal = getaddrinfo(pAddr, strPort, &addrCriteria, &servAddr);
	if (rtnVal != 0 || servAddr == NULL) {
		Log("getaddrinfo failed, addr=%s:%d, error=%d:%s", pAddr, port, errno,
				strerror(errno));
		return 0 - errno;
	}

	int fd = -1;
	int err = 0;
	struct addrinfo *addr_info;
	int nodelay = 1;
	int keepalive = 1;
	for (addr_info = servAddr; addr_info != NULL; addr_info =
			addr_info->ai_next) {
		// Create a reliable, stream socket using TCP
		fd = socket(addr_info->ai_family, addr_info->ai_socktype,
				addr_info->ai_protocol);
		if (fd < 0) {
			err = errno;
			continue; // Socket creation failed; try next address
		} else if ((setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay,
				sizeof(nodelay))) < 0) {
			Log("nodelay %d failed, errer = %d:%s", fd, errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive,
				sizeof(keepalive))) < 0) {
			Log("keepalive %d failed, errer = %d:%s", fd, errno,
					strerror(errno));
			err = errno;
		} else if (::connect(fd, addr_info->ai_addr, addr_info->ai_addrlen)
				< 0) { // Establish the connection to the echo server
			err = errno;
			Log("connect %d failed, error=%d:%s", fd, errno, strerror(errno));
		} else {
			err = 0;
			break;
		}

		close(fd); // Socket connection failed; try next address
		fd = -1;
	}
	freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()

	if (fd <= 0) {
		Log("connect failed to %s:%d, error=%d:%s", pAddr, port, err,
				strerror(err));
		return -1;
	}

	m_Fd = fd;
	if (!block) {
		int flags = fcntl(fd, F_GETFL, 0);
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	return fd;
}

int ClientSocket::GetSocketFd() const {
	return m_Fd;
}

bool ClientSocket::IsConnected() const {
	return m_Fd > 0;
}

void ClientSocket::Close() {
	if (m_Fd > 0) {
		Log("ClientSocket :] close : [%d]", m_Fd);
		close(m_Fd);
		m_Fd = 0;
	}
}

int ClientSocket::ReadData(std::queue<std::string> &q) throw (utils::Exception) {
	if (!IsConnected()) {
		throw utils::Exception("disconnected from socket server");
	}
	char _buffer[BUFFER_SIZE] = { 0 };
	memset(_buffer, 0, BUFFER_SIZE);
	ssize_t numBytesRcvd = recv(m_Fd, _buffer, BUFFER_SIZE, 0);
	Log("read original data size [%ld]", numBytesRcvd);
	if (numBytesRcvd <= 0) { //first read, lt 0 means client was closed
		return -1;
	}
	Log("read original data [%s]", _buffer);
	m_ReadBuffer.append(_buffer, numBytesRcvd);
	Log("current buffer [%s]", m_ReadBuffer.c_str());

	int counter = 0;
	while (true) {
		int index = utils::StringUtils::FindFirstSymbol(":", m_ReadBuffer);
		if (index >= 0) {
			//parse message length
			std::string lenStr = utils::StringUtils::HeadSubString(m_ReadBuffer,
					index).c_str();
			int length = atoi(lenStr.c_str());
			if (length <= 0) { //illegal message header
				Log("Illegal message header : [%s], drop!", lenStr.c_str());
				return -1;
			} else {
				//exclude header
				if ((unsigned int) length
						<= (m_ReadBuffer.size() - lenStr.size()/* message length */
						- 1/* ":" */)) {
					utils::StringUtils::ShiftStringLeft(m_ReadBuffer,
							index + 1);
				} else {
					break;
				}
			}

			//parse message body
			std::string data = utils::StringUtils::HeadSubString(m_ReadBuffer,
					length);
			utils::StringUtils::ShiftStringLeft(m_ReadBuffer, length);
			q.push(data);
			counter++;
		} else {
			break;
		}
	}
	Log("parsed %d messages this time", counter);
	Log("after buffer parsed [%s]", m_ReadBuffer.c_str());
	return 0;
}

int ClientSocket::WriteData(const std::string &data) throw (utils::Exception) {
	if (!IsConnected()) {
		throw utils::Exception("disconnected from socket server");
	}

	const char *_data = data.c_str();
	int size = data.size();
	int numBytes = 0;
	do {
		int sent = send(m_Fd, _data + numBytes, size - numBytes, 0);
		if (sent < 0) {
			Log("WriteData to %d failed, errer = %d:%s", m_Fd, errno,
					strerror(errno));
			return sent;
		}
		numBytes += sent;
	} while (numBytes < size);

	return numBytes;
}

}

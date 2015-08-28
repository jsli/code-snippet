/*
 * ServerDaemonImpl.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include <sys/socket.h>

#include "ota/daemon/ServerDaemonImpl.h"
#include "utils/Logger.h"
#include "json/json.h"
#include "ota/message/OtaMessage.h"
#include "ota/message/SystemMessage.h"
#include "ota/config/OtaConfig.h"

namespace ota {

const std::string EVENT_CLEAR_MSG = "EVENT_CLEAR_MSG";
const std::string EVENT_UPGRADE = "EVENT_UPGRADE";
const std::string EVENT_DOWNLOAD = "EVENT_DOWNLOAD";
const std::string RESULT_SUCCESS = "SUCCESS";
const std::string RESULT_FAIL = "FAIL";

const int QUEUE_SIZE = 128;

ServerDaemonImpl::ServerDaemonImpl(int port) :
		ServerDaemon(port), EventTarget() {
	// TODO Auto-generated constructor stub
	m_FirstConnection = true;
	m_HasUpdateResult = false;
	m_UpdateResult = false;
	m_HomeConnection = NULL;
	m_MessageQueue = new utils::MutexQueue<IMessage>(QUEUE_SIZE);
}

ServerDaemonImpl::~ServerDaemonImpl() {
	// TODO Auto-generated destructor stub
	while (m_MessageQueue->Size() > 0) {
		IMessage * msg = m_MessageQueue->Front();
		delete msg;
		m_MessageQueue->Pop();
	}
	delete m_MessageQueue;
}

void ServerDaemonImpl::onMessageReceived(sock::ClientSocket *client,
		const std::string &message) {
	Log("received message : [%s]", message.c_str());
//	do {
//		Log("check HANDSHAKE");
//		Json::Reader reader;
//		Json::StyledWriter styled_writer;
//		Json::Value val;
//		if (!reader.parse(message, val)) {
//			break;
//		}
//
//		if (val["type"] != "HANDSHAKE" || val["data"] != "ota_client") {
//			break;
//		}
//
//		if (m_HomeConnection != NULL) {
//			m_HomeConnection->Close();
//		}
//		m_HomeConnection = client;
//
//		if (m_FirstConnection && m_HasUpdateResult) {
//			m_FirstConnection = false;
//			m_MessageQueue->Push(
//					new OtaMessage(EVENT_UPGRADE,
//							m_UpdateResult ? RESULT_SUCCESS : RESULT_FAIL));
//		} else {
//			m_MessageQueue->Push(new SystemMessage());
//		}
//		return;
//	} while (false);
//	client->Close();
}

void ServerDaemonImpl::onNewConnection(sock::ClientSocket *conn) {
	Log("received new connection : [%d]", conn->GetSocketFd());
	if (m_HomeConnection != NULL) {
		m_HomeConnection->Close();
	}
	m_HomeConnection = conn;
	m_MessageQueue->Push(new SystemMessage());
}

void ServerDaemonImpl::onCloseConnection(sock::ClientSocket *conn) {
	Log("received closed connection : [%d]", conn->GetSocketFd());
}

void ServerDaemonImpl::onReadError(sock::ClientSocket *conn) {
	Log("received session read error : [%d]", conn->GetSocketFd());
}

void ServerDaemonImpl::onEvent(const std::string &type,
		const std::string &message) {
	Log("ServerDaemonImpl onEvent, [%s]-[%s]", type.c_str(), message.c_str());
	Json::Reader reader;
	Json::Value data;
	if (!reader.parse(message, data)) {
		return;
	}

	if (NOTIFICATION_EVENT == type) {
		IMessage *msg = NULL;
		std::string _message = data["message"].asString();
		if ("reboot" == _message) {
			msg = new OtaMessage(EVENT_DOWNLOAD, RESULT_SUCCESS);
		} else if ("clear" == _message) {
			msg = new OtaMessage(EVENT_CLEAR_MSG, "");
		} else if ("result" == _message) {
			m_HasUpdateResult = true;
			m_UpdateResult = data["is_success"].asBool();
			msg = new OtaMessage(EVENT_UPGRADE,
					m_UpdateResult ? RESULT_SUCCESS : RESULT_FAIL);
		} else if ("system_status" == _message) {
			msg = new SystemMessage();
		}

		if (msg != NULL) {
			m_MessageQueue->Push(msg);
		}
	}
}

void ServerDaemonImpl::flushMessage() {
	if (m_HomeConnection == NULL) {
		return;
	}

	while (m_MessageQueue->Size() > 0) {
		IMessage * msg = m_MessageQueue->Front();
		std::string _msg = msg->GetMessage();
		delete msg;
		m_MessageQueue->Pop();
		if (m_HomeConnection->IsConnected()) {
			Log("notify home -> %s", _msg.c_str());
			m_HomeConnection->WriteData(_msg);
		} else {
			Log("notify home failed, reason: socket closed.");
		}
	}
}

std::string ServerDaemonImpl::GetName() {
	return "ServerDaemonImpl";
}

}

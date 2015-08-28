/*
 * OtaDaemon.cpp
 *
 *  Created on: 2014-8-21
 *      Author: manson
 */

#include <sys/types.h>

#include "ota/daemon/OtaDaemon.h"
#include "event/EventLooper.h"
#include "utils/Logger.h"
#include "json/json.h"
#include "ota/config/OtaConfig.h"
#include "utils/FsUtils.h"
#include "platform/RecoveryManager.h"

namespace ota {

OtaDaemon::OtaDaemon() :
		EventTarget() {
	// TODO Auto-generated constructor stub
	m_ServerDaemonImpl = new ServerDaemonImpl(SOCKET_PORT);
	m_ServerDaemon = m_ServerDaemonImpl;
	m_EventTarget = m_ServerDaemonImpl;
	m_CheckDaemon = new CheckDaemon();
}

OtaDaemon::~OtaDaemon() {
	// TODO Auto-generated destructor stub
	delete m_CheckDaemon;
	delete m_ServerDaemonImpl;
}

void OtaDaemon::Start() throw (utils::Exception) {
	Log("OtaDaemon Start...[%d]", gettid());

	if (!init()) {
		throw utils::Exception("OtaDaemon init failed, abort!");
	}
	checkLastUpdate();

	event::EventLooper *looper = event::EventLooper::GetInstance();
	looper->RegisterListener(this, NOTIFICATION_EVENT, m_EventTarget);
	looper->RegisterListener(m_CheckDaemon->GetEventTarget(), DOWNLOAD_EVENT,
			this);
	m_ServerDaemon->Start();
	m_CheckDaemon->Start();
	looper->Loop(false);
}

bool OtaDaemon::init() {
	int counter = 0;
	while (counter < 15) {
		if (utils::FsUtils::MkDir(CACHE_ROOT)) {
			return true;
		}
		Log("check %s failed, retry %d", CACHE_ROOT.c_str(), counter);
		counter++;
		sleep(1);
	}
	return false;
}

void OtaDaemon::checkLastUpdate() {
	bool need_system_status = true;
	bool update_success = platform::RecoveryManager::IsRecoverySuccess();
	if (update_success) {
		need_system_status = false;
		Json::Value data;
		data["message"] = "result";
		data["is_success"] = true;
		sendNotification(data.toString());
		utils::FsUtils::CleanDir(CACHE_ROOT);
		Log("update success, delete %s", CACHE_ROOT.c_str());
	}
	bool update_failed = platform::RecoveryManager::IsRecoveryFailed();
	if (update_failed) {
		need_system_status = false;
		Json::Value data;
		data["message"] = "result";
		data["is_success"] = false;
		sendNotification(data.toString());
		utils::FsUtils::CleanDir(CACHE_ROOT);
		Log("update failed, delete %s", CACHE_ROOT.c_str());
	}
	if (need_system_status) {
		Json::Value data;
		data["message"] = "system_status";
		sendNotification(data.toString());
	}
}

void OtaDaemon::sendNotification(const std::string &notification) {
	SendEvent(NOTIFICATION_EVENT, notification);
}

void OtaDaemon::onEvent(const std::string &type, const std::string &message) {
	Log("OtaDaemon onEvent, [type:%s]-[message:%s]", type.c_str(),
			message.c_str());
	Json::Reader reader;
	Json::Value data;
	if (!reader.parse(message, data)) {
		return;
	}

	if (DOWNLOAD_EVENT == type) {
		std::string _message = data["message"].asString();
		if ("start" == _message || "failed" == _message) {
			platform::RecoveryManager::ClearRecoveryCommand();
			Json::Value _data;
			_data["message"] = "clear";
			sendNotification(_data.toString());
			if ("failed" == _message) {
				utils::FsUtils::CleanDir(CACHE_ROOT);
			}
		} else if ("success" == _message) {
			platform::RecoveryManager::WriteRecoveryCommand(
					data["path"].asString(), false);
			Json::Value _data;
			_data["message"] = "reboot";
			sendNotification(_data.toString());
		} else if ("retry" == _message) {
		}
	}
}

std::string OtaDaemon::GetName() {
	return "OtaDaemon";
}

}

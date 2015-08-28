/*
 * DeviceInfo.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include "platform/DeviceInfo.h"
#include "platform/SystemProperty.h"
#include "utils/Logger.h"

namespace platform {

DeviceInfo * DeviceInfo::m_Instance = NULL;

DeviceInfo::DeviceInfo() {
	// TODO Auto-generated constructor stub
//	load();
	loadDefault();
}

DeviceInfo::~DeviceInfo() {
	// TODO Auto-generated destructor stub
}

void DeviceInfo::load() {
	m_Info["serial_number"] = SystemProperty::GetValueFromProcFile(
			"/proc/rknand", "SN =", strlen("SN =") + 1, "unknow");
	m_Info["product_name"] = SystemProperty::GetProperty("ro.product.name",
			"unknow");
	m_Info["manufacturer"] = SystemProperty::GetProperty(
			"ro.product.manufacturer", "unknow");
	m_Info["hardware_version"] = SystemProperty::GetValueFromProcFile(
			"/proc/rknand", "HWINF =", strlen("HWINF =") + 1, "unknow");
	m_Info["version"] = SystemProperty::GetProperty("ro.product.version",
			"unknow");
	m_Info["mac"] = SystemProperty::GetValueFromProcFile(
			"/sys/class/net/wlan0/address", "unknow");
	// this maybe failed when otad first launched since network is bad
	// m_Info["ip"] = u_getDeviceIp();
	m_Info["platform"] = SystemProperty::GetProperty("ro.product.platform",
			"unknow");
}

void DeviceInfo::loadDefault() {
	m_Info["serial_number"] = "unknow";
	m_Info["product_name"] = "rk30sdk";
	m_Info["manufacturer"] = "rockchip";
	m_Info["hardware_version"] = "unknow";
	m_Info["version"] = "201408010000";
	m_Info["mac"] = "98:3b:16:bb:44:e4";
	m_Info["platform"] = "MatchStick";
}

DeviceInfo * DeviceInfo::GetInstance() {
	if (DeviceInfo::m_Instance == NULL) {
		DeviceInfo::m_Instance = new DeviceInfo();
	}
	return DeviceInfo::m_Instance;
}

std::string DeviceInfo::GetDeviceInfo() {
	return m_Info.toString();
}

void DeviceInfo::LogSelf() {
	Log("%s", GetDeviceInfo().c_str());
}

}

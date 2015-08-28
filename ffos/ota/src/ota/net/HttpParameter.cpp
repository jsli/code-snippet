/*
 * HttpParameter.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include "ota/net/HttpParameter.h"
#include "platform/DeviceInfo.h"
#include "utils/Logger.h"

namespace ota {

HttpParameter * HttpParameter::m_Instance = NULL;

HttpParameter::HttpParameter() {
	// TODO Auto-generated constructor stub
	load();
}

HttpParameter::~HttpParameter() {
	// TODO Auto-generated destructor stub
}

void HttpParameter::load() {
	Json::Reader reader;
	Json::Value device_info;
	reader.parse(platform::DeviceInfo::GetInstance()->GetDeviceInfo(),
			device_info);
	m_Parameter["device_info"] = device_info;
	m_Parameter["code"] = "10000";
}

HttpParameter * HttpParameter::GetInstance() {
	if (HttpParameter::m_Instance == NULL) {
		HttpParameter::m_Instance = new HttpParameter();
	}
	return HttpParameter::m_Instance;
}

std::string HttpParameter::GetParameters() {
	return m_Parameter.toString();
}

void HttpParameter::LogSelf() {
	Log("%s", GetParameters().c_str());
}

}

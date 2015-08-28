/*
 * SystemMessage.cpp
 *
 *  Created on: 2014-6-28
 *      Author: manson
 */

#include <sstream>

#include "ota/message/SystemMessage.h"
#include "platform/SystemProperty.h"

namespace ota {

SystemMessage::SystemMessage() {
	m_Type = "SYSTEM_STATUS";

	Init();
	JoinElements();
}

SystemMessage::~SystemMessage() {
}

void SystemMessage::Init() {
	m_Version = platform::SystemProperty::GetProperty("ro.product.version",
			"unknow version");
	m_Platform = platform::SystemProperty::GetProperty("ro.product.platform",
			"unknow platform");

	std::stringstream ss;
	ss << "{";
	ss << "\"version\"";
	ss << ":";
	ss << "\"";
	ss << m_Version;
	ss << "\"";
	ss << ", ";
	ss << "\"platform\"";
	ss << ":";
	ss << "\"";
	ss << m_Platform;
	ss << "\"";
	ss << "}";

	m_Data = ss.str();
}

}

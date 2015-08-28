/*
 * OtaMessage.cpp
 *
 *  Created on: 2014-6-7
 *      Author: manson
 */

#include <sstream>

#include "ota/message/OtaMessage.h"
#include "platform/SystemProperty.h"

namespace ota {

OtaMessage::OtaMessage(const std::string &event, const std::string &result) {
	m_Type = "OTA_UPDATE";
	m_Event = event;
	m_Result = result;

	Init();
	JoinElements();
}

OtaMessage::~OtaMessage() {
}

void OtaMessage::Init() {
	std::stringstream ss;

	ss << "{";
	ss << "\"event\"";
	ss << ":";
	ss << "\"";
	ss << m_Event;
	ss << "\"";
	ss << ", ";
	ss << "\"result\"";
	ss << ":";
	ss << "\"";
	ss << m_Result;
	ss << "\"";

//	if (m_Result == "SUCCESS") {
	std::string version = platform::SystemProperty::GetProperty(
			"ro.product.version", "unknow version");
	std::string platform = platform::SystemProperty::GetProperty(
			"ro.product.platform", "unknow platform");

	ss << ", ";
	ss << "\"version\"";
	ss << ":";
	ss << "\"";
	ss << version;
	ss << "\"";
	ss << ", ";
	ss << "\"platform\"";
	ss << ":";
	ss << "\"";
	ss << platform;
	ss << "\"";
//	}

	ss << "}";

	m_Data = ss.str();
}

}

/*
 * Message.cpp
 *
 *  Created on: 2014-6-28
 *      Author: manson
 */

#include <sstream>

#include "ota/message/Message.h"
#include "json/json.h"
#include "utils/Logger.h"

namespace ota {

Message::Message() {
}

Message::~Message() {
}

std::string Message::GetMessage() {
	return m_Message;
}

std::string Message::GetType() {
	return m_Type;
}

std::string Message::GetData() {
	return m_Data;
}

void Message::JoinElements() {
	std::stringstream ss;

	ss << "{";
	ss << "\"type\"";
	ss << ":";
	ss << "\"";
	ss << m_Type;
	ss << "\"";
	ss << ", ";
	ss << "\"data\"";
	ss << ":";
	ss << m_Data;
	ss << "}";
	std::string msg = ss.str();

	ss.str("");
	ss << msg.size();
	ss << ":";
	ss << msg;

	m_Message = ss.str();

	if (CHECK_JSON_FORMAT) {
		Json::Reader reader;
		Json::StyledWriter styled_writer;
		Json::Value val;
		if (!reader.parse(m_Message, val)) {
			Log("Illegal json format : %s", m_Message.c_str());
		} else {
			Log("Good json format : %s", m_Message.c_str());
		}
	}
}

void Message::LogSelf() {
	Log("message = %s", m_Message.c_str());
}

}

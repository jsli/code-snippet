/*
 * Event.cpp
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#include "event/Event.h"

namespace event {

Event::Event(const std::string &message, const std::string &type,
		long eventSenderId) {
	m_Type = type;
	m_Message = message;
	m_EventSenderId = eventSenderId;
}

Event::~Event() {
}

std::string Event::GetType() {
	return m_Type;
}

std::string Event::GetMessage() {
	return m_Message;
}

long Event::GetEventSenderId() {
	return m_EventSenderId;
}

}

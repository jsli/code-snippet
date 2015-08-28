/*
 * EventTarget.cpp
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#include "event/EventTarget.h"
#include "event/EventLooper.h"

namespace event {

EventTarget::EventTarget() {
	m_Id = (long) this;
}

EventTarget::~EventTarget() {
}

long EventTarget::GetId() {
	return m_Id;
}

std::string EventTarget::GetName() {
	return "EventTarget";
}

bool EventTarget::listenEvent(EventTarget * sender,
		const std::string &eventType) {
	if (sender == NULL || eventType.size() == 0) {
		return false;
	}
	return EventLooper::GetInstance()->RegisterListener(sender, eventType, this);
}

bool EventTarget::unListenEvent(EventTarget * sender,
		const std::string &eventType) {
	if (sender == NULL || eventType.size() == 0) {
		return false;
	}
	return EventLooper::GetInstance()->UnregisterListener(sender, eventType,
			this);
}

bool EventTarget::SendEvent(const std::string &type) {
	if (type.size() == 0) {
		return false;
	}
	return SendEvent(type, "");
}

bool EventTarget::SendEvent(const std::string &type,
		const std::string &message) {
	if (type.size() == 0 || message.size() == 0) {
		return false;
	}
	return EventLooper::GetInstance()->QueueEvent(this, type, message);
}

void EventTarget::onEvent(const std::string &type, const std::string &message) {
}

}

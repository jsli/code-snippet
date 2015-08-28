/*
 * EventLooper.cpp
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#include "event/EventLooper.h"
#include "utils/Logger.h"
#include "utils/Exception.h"

namespace event {

EventLooper::EventLooper() :
		Thread(true) {
	m_InLoop = false;
	m_EventQueue = new utils::MutexQueue<Event>(1024);
	m_ListenersList = new std::vector<EventLooper::Listener *>();

	pthread_mutex_init(&m_ListenerLock, NULL);
}

EventLooper::~EventLooper() {
	while (m_EventQueue->Size() > 0) {
		Event * event = m_EventQueue->Front();
		delete event;
		m_EventQueue->Pop();
	}
	delete m_EventQueue;

	pthread_mutex_lock(&m_ListenerLock);
	while (m_ListenersList->begin() != m_ListenersList->end()) {
		EventLooper::Listener * listener = m_ListenersList->front();
		m_ListenersList->erase(m_ListenersList->begin());
		delete listener;
	}
	m_ListenersList->clear();
	delete m_ListenersList;
	pthread_mutex_unlock(&m_ListenerLock);
	pthread_mutex_destroy(&m_ListenerLock);
}

EventLooper * EventLooper::m_Instance = NULL;

EventLooper * EventLooper::GetInstance() {
	if (!m_Instance) {
		m_Instance = new EventLooper();
	}
	return m_Instance;
}

void EventLooper::Loop(bool detached) throw (utils::Exception) {
	if (detached) {
		if (Start() != 0) {
			throw utils::Exception("EventLooper loop error, detached mode");
		}
	} else {
		doLoop();
		throw utils::Exception("EventLooper loop error, not detached mode");
	}
}

void EventLooper::doLoop() {
	m_InLoop = true;
	while (true) {
		Event * event = m_EventQueue->Front();
		m_EventQueue->Pop();
		int size = m_ListenersList->size();
		for (int i = 0; i < size; i++) {
			EventLooper::Listener *listener = m_ListenersList->at(i);
			EventTarget *eventSender = listener->eventSender;
			EventTarget *eventHandler = listener->eventHandler;
			std::string eventType = listener->eventType;
			if (eventSender == NULL || eventHandler == NULL) {
				continue;
			}
			if (eventSender->GetId() == event->GetEventSenderId()
					&& eventType == event->GetType()) {
				try {
					eventHandler->onEvent(event->GetType(),
							event->GetMessage());
				} catch (utils::Exception &e) {
					e.Print();
				}
			}
		}
		delete event;
	}
}

void EventLooper::Run() {
	doLoop();
}

EventLooper::Listener * EventLooper::findListener(EventTarget * sender,
		const std::string &eventType, EventTarget * handler) {
	int size = m_ListenersList->size();
	for (int i = 0; i < size; i++) {
		EventLooper::Listener * listener = m_ListenersList->at(i);
		if (listener->eventSender->GetId() == sender->GetId()
				&& eventType == listener->eventType
				&& listener->eventHandler->GetId() == handler->GetId()) {
			return listener;
		}
	}
	return NULL;
}

bool EventLooper::removeListener(EventTarget * sender,
		const std::string &eventType, EventTarget * handler) {
	int size = m_ListenersList->size();
	for (int i = 0; i < size; i++) {
		EventLooper::Listener *listener = m_ListenersList->at(i);
		if (listener->eventSender->GetId() == sender->GetId()
				&& eventType == listener->eventType
				&& listener->eventHandler->GetId() == handler->GetId()) {
			m_ListenersList->erase(m_ListenersList->begin() + i);
			delete listener;
			return true;
		}
	}
	return false;
}

bool EventLooper::RegisterListener(EventTarget * sender,
		const std::string &eventType, EventTarget * handler) {
	if (m_InLoop) {
		return false;
	}
	if (!sender || !handler || eventType.size() == 0) {
		//illegal parameters
		return false;
	}
	Log("register: [%s] ---listen---> [%s]", handler->GetName().c_str(),
			sender->GetName().c_str());
	pthread_mutex_lock(&m_ListenerLock);
	EventLooper::Listener *lisener = findListener(sender, eventType, handler);
	if (lisener == NULL) {
		lisener = new EventLooper::Listener();
		lisener->eventSender = sender;
		lisener->eventType = eventType;
		lisener->eventHandler = handler;
		m_ListenersList->push_back(lisener);
	}
	Log("after register listeners size = %d", m_ListenersList->size());
	pthread_mutex_unlock(&m_ListenerLock);
	return true;
}

bool EventLooper::UnregisterListener(EventTarget * sender,
		const std::string &eventType, EventTarget * handler) {
	if (m_InLoop) {
		return false;
	}
	if (!sender || !handler || eventType.size() == 0) {
		//illegal parameters
		return false;
	}
	Log("unregister: [%s] ---unlisten---> [%s]", handler->GetName().c_str(),
			sender->GetName().c_str());
	pthread_mutex_lock(&m_ListenerLock);
	bool res = removeListener(sender, eventType, handler);
	Log("listeners size = %d", m_ListenersList->size());
	pthread_mutex_unlock(&m_ListenerLock);
	return res;
}

bool EventLooper::QueueEvent(EventTarget * sender, std::string type,
		std::string message) {
	Event * event = new Event(message, type, sender->GetId());
	Log("[%s] send event : [type:%s] - [message:%s]", sender->GetName().c_str(),
			type.c_str(), message.c_str());
	m_EventQueue->Push(event);
	Log("current event queue size: %d", m_EventQueue->Size());
	return true;
}

}

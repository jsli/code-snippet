/*
 * EventLooper.h
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#ifndef EVENTLOOPER_H_
#define EVENTLOOPER_H_

#include <vector>
#include <pthread.h>

#include "utils/MutexQueue.h"
#include "utils/Exception.h"
#include "thread/Thread.h"
#include "event/Event.h"
#include "event/EventTarget.h"

namespace event {

class EventLooper: public thread::Thread {
private:
	bool m_InLoop;
	utils::MutexQueue<Event> * m_EventQueue;

	struct Listener {
		EventTarget * eventSender;
		std::string eventType;
		EventTarget * eventHandler;
	};
	pthread_mutex_t m_ListenerLock;
	std::vector<EventLooper::Listener *> * m_ListenersList;

	EventLooper::Listener * findListener(EventTarget * sender,
			const std::string &eventType, EventTarget * handler);
	bool removeListener(EventTarget * sender, const std::string &eventType,
			EventTarget * handler);

	static EventLooper * m_Instance;
	EventLooper();

protected:
	virtual void Run();
	void doLoop();

public:
	static EventLooper * GetInstance();
	virtual ~EventLooper();

	void Loop(bool detached) throw (utils::Exception);

	bool RegisterListener(EventTarget * sender, const std::string &eventType,
			EventTarget * handler);
	bool UnregisterListener(EventTarget * sender, const std::string &eventType,
			EventTarget * handler);
	bool QueueEvent(EventTarget * sender, std::string type,
			std::string message);
};

}

#endif /* EVENTLOOPER_H_ */

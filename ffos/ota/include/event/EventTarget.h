/*
 * EventTarget.h
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#ifndef EVENTTARGET_H_
#define EVENTTARGET_H_

#include <string>

#include "event/Event.h"

namespace event {

/**
 * as event sender and receiver
 */
class EventTarget {
private:
	long m_Id;

protected:
	bool listenEvent(EventTarget * sender, const std::string &eventType);
	bool unListenEvent(EventTarget * sender, const std::string &eventType);

public:
	EventTarget();
	virtual ~EventTarget();

	long GetId();
	virtual std::string GetName();

	//sender methods
	bool SendEvent(const std::string &type);
	bool SendEvent(const std::string &type, const std::string &message);

	//receiver methods
	virtual void onEvent(const std::string &type, const std::string &message);
};

}

#endif /* EVENTTARGET_H_ */

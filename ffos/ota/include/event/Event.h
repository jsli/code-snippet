/*
 * Event.h
 *
 *  Created on: 2014-7-21
 *      Author: manson
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <string>

namespace event {

class Event {
private:
	std::string m_Type;
	std::string m_Message;
	long m_EventSenderId;

public:
	Event(const std::string &message, const std::string &type = "",
			long eventSenderId = -1);
	virtual ~Event();

	long GetEventSenderId();
	std::string GetType();
	std::string GetMessage();
};

}

#endif /* EVENT_H_ */

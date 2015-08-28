/*
 * OtaMessage.h
 *
 *  Created on: 2014-6-7
 *      Author: manson
 */

#ifndef OTAMESSAGE_H_
#define OTAMESSAGE_H_

#include "Message.h"

namespace ota {

#define RETRY_COUNT			5
#define RETRY_INTERVAL		10

class OtaMessage: public Message {
private:
	std::string m_Event;
	std::string m_Result;

protected:
	void Init();

public:
	OtaMessage(const std::string &event, const std::string &result);
	virtual ~OtaMessage();
	bool CanResent();
};

}

#endif /* OTAMESSAGE_H_ */

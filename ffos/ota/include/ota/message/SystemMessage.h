/*
 * SystemMessage.h
 *
 *  Created on: 2014-6-28
 *      Author: manson
 */

#ifndef SYSTEMMESSAGE_H_
#define SYSTEMMESSAGE_H_

#include "Message.h"

namespace ota {

class SystemMessage: public Message {
private:
	std::string m_Version;
	std::string m_Platform;

protected:
	void Init();

public:
	SystemMessage();
	virtual ~SystemMessage();
};

}

#endif /* SYSTEMMESSAGE_H_ */

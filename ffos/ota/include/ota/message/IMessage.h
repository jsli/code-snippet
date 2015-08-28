/*
 * IMessage.h
 *
 *  Created on: 2014-6-28
 *      Author: manson
 */

#ifndef IMESSAGE_H_
#define IMESSAGE_H_

#include <string>

namespace ota {

class IMessage {
public:
	IMessage() {
	}
	virtual ~IMessage() {
	}
	virtual std::string GetMessage() = 0;
	virtual void LogSelf() = 0;
};

}

#endif /* IMESSAGE_H_ */

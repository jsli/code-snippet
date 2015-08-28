/*
 * HttpParameter.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef HTTPPARAMETER_H_
#define HTTPPARAMETER_H_

#include <string>
#include "json/json.h"

namespace ota {

class HttpParameter {
private:
	static HttpParameter * m_Instance;
	Json::Value m_Parameter;

	HttpParameter();
	void load();

public:
	virtual ~HttpParameter();

	static HttpParameter * GetInstance();

	std::string GetParameters();
	void LogSelf();
};

}

#endif /* HTTPPARAMETER_H_ */

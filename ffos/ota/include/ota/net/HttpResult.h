/*
 * HttpResult.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef HTTPRESULT_H_
#define HTTPRESULT_H_

#include <string>
#include "json/json.h"

namespace ota {

class HttpResult {
private:
	Json::Value m_Result;

public:
	HttpResult();
	virtual ~HttpResult();

	bool Load(const std::string &result);
	std::string ToString();
	void LogSelf();

	int GetStatus();
	std::string GetUrl();
	std::string GetCode();
	std::string GetMd5();
	std::string GetFileName();
	double GetFileSize();
};

}

#endif /* HTTPRESULT_H_ */

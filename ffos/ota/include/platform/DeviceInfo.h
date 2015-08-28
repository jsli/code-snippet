/*
 * DeviceInfo.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef DEVICEINFO_H_
#define DEVICEINFO_H_

#include <string>

#include "json/json.h"

namespace platform {

class DeviceInfo {
private:
	static DeviceInfo * m_Instance;
	Json::Value m_Info;

	DeviceInfo();
	void load();
	void loadDefault();

public:
	virtual ~DeviceInfo();

	static DeviceInfo * GetInstance();

	std::string GetDeviceInfo();
	void LogSelf();
};

}

#endif /* DEVICEINFO_H_ */

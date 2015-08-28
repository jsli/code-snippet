/*
 * SystemProperty.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include <iostream>
#include <fstream>

#include "platform/SystemProperty.h"
#include <cutils/properties.h>

namespace platform {

SystemProperty::SystemProperty() {
	// TODO Auto-generated constructor stub

}

SystemProperty::~SystemProperty() {
	// TODO Auto-generated destructor stub
}

std::string SystemProperty::GetProperty(const std::string &key,
		const std::string &defaultValue) {
	char property[256];
	memset(property, 0, 256);
	property_get(key.c_str(), property, defaultValue.c_str());
	return std::string(property);
}

void SystemProperty::SetProperty(const std::string &key,
		const std::string &value) {
	property_set(key.c_str(), value.c_str());
}

std::string SystemProperty::GetValueFromProcFile(const std::string &path,
		const std::string &defaultValue) {
	std::string buffer;
	std::ifstream ifs;
	ifs.open(path.c_str(), std::ios_base::in);
	getline(ifs, buffer, '\n');
	if (buffer.length() > 0) {
		return buffer;
	}
	ifs.close();
	return defaultValue;
}

std::string SystemProperty::GetValueFromProcFile(const std::string &path,
		const std::string &prefix, int prefixLength,
		const std::string &defaultValue) {
	std::string buffer;
	std::string value;
	std::ifstream ifs;
	ifs.open(path.c_str(), std::ios_base::in);
	while (getline(ifs, buffer, '\n')) {
		unsigned int pos = buffer.find(prefix);

		if (std::string::npos != pos) {
			value = buffer.substr(pos + prefixLength, buffer.size());
		}
	}
	ifs.close();
	return value.size() > 0 ? value : defaultValue;
}

}

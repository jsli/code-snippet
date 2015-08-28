/*
 * SystemProperty.h
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#ifndef SYSTEMPROPERTY_H_
#define SYSTEMPROPERTY_H_

#include <string>

namespace platform {

class SystemProperty {
public:
	SystemProperty();
	virtual ~SystemProperty();

	static std::string GetProperty(const std::string &key,
			const std::string &defaultValue);
	static void SetProperty(const std::string &key, const std::string &value);

	static std::string GetValueFromProcFile(const std::string &path,
			const std::string &defaultValue);
	static std::string GetValueFromProcFile(const std::string &path,
			const std::string &prefix, int prefixLength,
			const std::string &defaultValue);
};

}

#endif /* SYSTEMPROPERTY_H_ */

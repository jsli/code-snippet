/*
 * ProcessMonitor.h
 *
 *  Created on: 2014-9-2
 *      Author: manson
 */

#ifndef PROCESSMONITOR_H_
#define PROCESSMONITOR_H_

#include <string>

namespace platform {

class ProcessMonitor {
private:
	static ProcessMonitor * m_Instance;
	ProcessMonitor();

public:
	static ProcessMonitor * GetInstance();
	virtual ~ProcessMonitor();

	bool CheckCertainProcessAlive(const std::string &name);
};

} /* namespace platform */
#endif /* PROCESSMONITOR_H_ */

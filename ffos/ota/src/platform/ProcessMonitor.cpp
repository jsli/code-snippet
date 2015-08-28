/*
 * ProcessMonitor.cpp
 *
 *  Created on: 2014-9-2
 *      Author: manson
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <vector>

#include "platform/ProcessMonitor.h"
#include "utils/Logger.h"
#include "ota/config/OtaConfig.h"

namespace platform {

ProcessMonitor * ProcessMonitor::m_Instance = NULL;

ProcessMonitor::ProcessMonitor() {
	// TODO Auto-generated constructor stub

}

ProcessMonitor::~ProcessMonitor() {
	// TODO Auto-generated destructor stub
}

ProcessMonitor * ProcessMonitor::GetInstance() {
	if (m_Instance == NULL) {
		m_Instance = new ProcessMonitor();
	}
	return m_Instance;
}

bool ProcessMonitor::CheckCertainProcessAlive(const std::string &name) {
	char queryCmd[128], queryResult[512];
	memset(queryCmd, 0, sizeof(queryCmd));
	sprintf(queryCmd, "b2g-ps | grep %s", name.c_str());
	Log("execute command: %s", queryCmd);

	FILE * pExecute = popen(queryCmd, "r");
	if (pExecute == NULL) {
		Log("execute command failed!");
		return false;
	}
	memset(queryResult, 0, sizeof(queryResult));
	fgets(queryResult, 512, pExecute);
	int resultLen = strlen(queryResult);
	if (resultLen == 0) {
		Log("process: %s not exist!\n", name.c_str());
		return false;
	}
	queryResult[resultLen - 1] = 0;
	Log("execute result: [%s]", queryResult);

	std::vector<std::string> info;
	char *p = strtok(queryResult, " ");
	while (p) {
		info.push_back(std::string(p));
		p = strtok(NULL, " ");
	}

	const char *pidStr = info.at(B2G_CASTAPPCONTAINER_INDEX).c_str();
	int pid = atoi(pidStr);
	Log("%s's pid is [%d]", name.c_str(), pid);
	if (pid == 0) {
		return false;
	}

	int ret = kill((pid_t) pid, 0);
	if (0 == ret) {
		Log("process: %s exist!\n", name.c_str());
	} else {
		Log("process: %s not exist!\n", name.c_str());
	}

	return true;
}

} /* namespace platform */

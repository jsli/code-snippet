/*
 * RecoveryManager.h
 *
 *  Created on: 2014-5-21
 *      Author: manson
 */

#ifndef RECOVERYMANAGER_H_
#define RECOVERYMANAGER_H_

#include <string>

namespace platform {

static const int MISC_PAGES = 3; // number of pages to save
static const int MISC_COMMAND_PAGE = 1; // bootloader command is this page

struct bootloader_message {
	char command[32];
	char status[32];
	char recovery[1024];
};

int installFotaUpdate(char *updatePath, int updatePathLength);

class RecoveryManager {
public:
	static int UpdateOta(std::string path, int pathLength);
	static void WriteRecoveryCommand(std::string path, bool wipe);
	static void ClearRecoveryCommand();
	static void WipeData();

	static bool IsRecoverySuccess();
	static bool IsRecoveryFailed();
};

}

#endif /* RECOVERYMANAGER_H_ */

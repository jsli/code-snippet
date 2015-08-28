/*
 * RecoveryManager.cpp
 *
 *  Created on: 2014-5-21
 *      Author: manson
 */

#include <string>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <cutils/android_reboot.h>

#include "platform/RecoveryManager.h"
#include "utils/FsUtils.h"
#include "utils/Logger.h"
#include "platform/mtd/mtdutils.h"

namespace platform {

/*copy from librecovery*/
#define RECOVERY_DIR "/cache/recovery"
const char kRecoveryCommand[] = RECOVERY_DIR "/command";
const char kLastInstall[] = RECOVERY_DIR "/last_install";
const char kWipeData[] = "--wipe_data";
const char kUpdatePackage[] = "--update_package";
const char kRebootRecovery[] = "recovery";
const int kWipeDataLength = sizeof(kWipeData) - 1;
const int kUpdatePackageLength = sizeof(kUpdatePackage) - 1;

static int set_bootloader_message_mtd(const struct bootloader_message *in) {
	size_t write_size;
	mtd_scan_partitions();
	const MtdPartition *part = mtd_find_partition_by_name("misc");
	if (part == NULL || mtd_partition_info(part, NULL, NULL, &write_size)) {
		Log("Can't find %s\n", "/misc");
		return -1;
	}

	MtdReadContext *read = mtd_read_partition(part);
	if (read == NULL) {
		Log("Can't open %s\n(%s)\n", "/misc", strerror(errno));
		return -1;
	}

	ssize_t size = write_size * MISC_PAGES;
	char data[size];
	ssize_t r = mtd_read_data(read, data, size);
	if (r != size)
		Log("Can't read %s\n(%s)\n", "/misc", strerror(errno));
	mtd_read_close(read);
	if (r != size)
		return -1;

	memcpy(&data[write_size * MISC_COMMAND_PAGE], in, sizeof(*in));

	MtdWriteContext *write = mtd_write_partition(part);
	if (write == NULL) {
		Log("Can't open %s\n(%s)\n", "/misc", strerror(errno));
		return -1;
	}
	if (mtd_write_data(write, data, size) != size) {
		Log("Can't write %s\n(%s)\n", "/misc", strerror(errno));
		mtd_write_close(write);
		return -1;
	}
	if (mtd_write_close(write)) {
		Log("Can't finish %s\n(%s)\n", "/misc", strerror(errno));
		return -1;
	}

	Log("Set boot command \"%s\"\n", in->command[0] != 255 ? in->command : "");
	return 0;
}

int RecoveryManager::UpdateOta(std::string path, int pathLength) {
	utils::FsUtils::DeleteFile("/cache/update.suc");
	return installFotaUpdate(strdup(path.c_str()), pathLength);
}

bool RecoveryManager::IsRecoverySuccess() {
	bool res = false;
	if (utils::FsUtils::IsFileExist("/cache/update.suc")) {
		utils::FsUtils::DeleteFile("/cache/update.suc");
		res = true;
	}
	return res;
}

bool RecoveryManager::IsRecoveryFailed() {
	bool res = false;
	if (utils::FsUtils::IsFileExist("/cache/update.err")) {
		utils::FsUtils::DeleteFile("/cache/update.err");
		res = true;
	}
	return res;
}

void RecoveryManager::WipeData() {
	struct bootloader_message boot;
	memset(&boot, 0, sizeof(boot));
	strlcpy(boot.command, "boot-recovery", sizeof(boot.command));
	char cmd[100] = "recovery\n--wipe_data";
	strlcpy(boot.recovery, cmd, sizeof(boot.recovery));

	set_bootloader_message_mtd(&boot);
}

void RecoveryManager::WriteRecoveryCommand(std::string path, bool wipe) {
	struct bootloader_message boot;
	memset(&boot, 0, sizeof(boot));
	strlcpy(boot.command, "boot-recovery", sizeof(boot.command));
	char cmd[256] = "recovery\n--update_package=";
	strcat(cmd, path.c_str());
	if (wipe) {
		strcat(cmd, "\n--wipe_data");
	}
	strlcpy(boot.recovery, cmd, sizeof(boot.recovery));
	Log("write misc: path=%s, wipe=%s, command=%s, status=%s, recovery=%s",
			path.c_str(), wipe ? "wipe_data" : "!wipe_data", boot.command,
			boot.status, boot.recovery);
	set_bootloader_message_mtd(&boot);
}

void RecoveryManager::ClearRecoveryCommand() {
	struct bootloader_message boot;
	memset(&boot, 0, sizeof(boot));
	set_bootloader_message_mtd(&boot);
}

/*copy from librecovery*/
static int safeWrite(FILE *file, const void *data, size_t size) {
	size_t written;

	do {
		written = fwrite(data, size, 1, file);
	} while (written == 0 && errno == EINTR);

	if (written == 0 && ferror(file)) {
		Log("Error writing data to file: %s", strerror(errno));
		return -1;
	}

	return written;
}

static int convertExternalStoragePath(char *srcPath, int srcPathLength,
		char *destPath) {
	char *extStorage = getenv("EXTERNAL_STORAGE");
	char *relPath;
	int extStorageLength, recoveryExtStorageLength;
	int relPathLength, destPathLength;

	if (!extStorage) {
		strlcpy(destPath, srcPath, srcPathLength + 1);
		return srcPathLength;
	}

	extStorageLength = strlen(extStorage);
	recoveryExtStorageLength = strlen("/sdcard");
	if (strncmp(srcPath, extStorage, extStorageLength) != 0) {
		strlcpy(destPath, srcPath, srcPathLength + 1);
		return srcPathLength;
	}

	relPath = srcPath + extStorageLength;
	relPathLength = srcPathLength - extStorageLength;

	destPathLength = recoveryExtStorageLength + relPathLength;
	strlcpy(destPath, "/sdcard", recoveryExtStorageLength + 1);
	strlcat(destPath, relPath, destPathLength + 1);
	return destPathLength;
}

static int execRecoveryCommand(char *command, size_t commandLength) {
	struct stat recoveryDirStat;
	FILE *commandFile;
	size_t bytesWritten;

	// Ensure the recovery directory exists
	if (mkdir(RECOVERY_DIR, 0770) == -1 && errno != EEXIST) {
		Log("Unable to create recovery directory \"%s\": %s", RECOVERY_DIR,
				strerror(errno));
		return -1;
	}

	commandFile = fopen(kRecoveryCommand, "w");
	if (!commandFile) {
		Log("Unable to open recovery command file \"%s\": %s", kRecoveryCommand,
				strerror(errno));
		return -1;
	}

	if (safeWrite(commandFile, command, commandLength) == -1
			|| safeWrite(commandFile, "\n", 1) == -1) {
		fclose(commandFile);
		return -1;
	}

	fclose(commandFile);

	// Reboot into the recovery partition
	Log("Rebooting into recovery: %s", command);

	return android_reboot(ANDROID_RB_RESTART2, 0, (char *) kRebootRecovery);
}

int installFotaUpdate(char *updatePath, int updatePathLength) {
	struct stat updateStat;
	char command[kUpdatePackageLength + 1 + 1024];
	int prefixLength;

	if (!updatePath) {
		Log("Error: null update path");
		return -1;
	}

	if (updatePathLength <= 0) {
		Log("Error: update path length invalid: %d", updatePathLength);
		return -1;
	}

	if (stat(updatePath, &updateStat) == -1) {
		Log("Error: could not stat update path \"%s\": %s", updatePath,
				strerror(errno));
		return -1;
	}

	if (!S_ISREG(updateStat.st_mode)) {
		Log("Error: update path \"%s\" is not a regular file", updatePath);
		return -1;
	}

	prefixLength = kUpdatePackageLength + 1;
	snprintf(command, prefixLength + 1, "%s=", kUpdatePackage);

	updatePathLength = convertExternalStoragePath(updatePath, updatePathLength,
			command + prefixLength);
	return execRecoveryCommand(command, prefixLength + updatePathLength);
}

}

/*
 * OtaConfig.h
 *
 *  Created on: 2014-8-23
 *      Author: manson
 */

#ifndef OTACONFIG_H_
#define OTACONFIG_H_

#include <string>

const std::string DOWNLOAD_EVENT = "download";
const std::string NOTIFICATION_EVENT = "notification";

const int SOCKET_PORT = 4999;
const std::string OTA_SERVER_API = "http://ota.infthink.com/cast/check";

const int CHECK_INTERVAL = 60;
const std::string CACHE_ROOT = "/mnt/sdcard/fota/";
const std::string DOWNLOAD_TASK_CACHE_FILE = CACHE_ROOT + "download_task.cache";

const int MAX_DOWNLOAD_SPEED_BUSY = 100; //limit speed when playing media
const int MAX_DOWNLOAD_SPEED_IDLE = 500;

const std::string B2G_CASTAPPCONTAINER_NAME = "CastAppContaine";
const int B2G_CASTAPPCONTAINER_INDEX = 3;

#endif /* OTACONFIG_H_ */

/*
 * main.cpp
 *
 *  Created on: 2014-8-21
 *      Author: manson
 */

#include "ota/daemon/OtaDaemon.h"
#include "utils/Logger.h"

using namespace ota;

int main() {
	OtaDaemon * daemon = new OtaDaemon();
	daemon->Start();
	Log("OTAD------------------------abort!");
	return 0;
}

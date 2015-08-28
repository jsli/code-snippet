/*
 * TimeUtils.cpp
 *
 *  Created on: 2014-7-19
 *      Author: manson
 */

#include <sys/time.h>

#include "utils/TimeUtils.h"

namespace utils {

TimeUtils::TimeUtils() {
	// TODO Auto-generated constructor stub
}

TimeUtils::~TimeUtils() {
	// TODO Auto-generated destructor stub
}

long TimeUtils::GetTimeMicroSec() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long time = tv.tv_sec * 1000 + (tv.tv_usec / 1000);
	return time;
}

}

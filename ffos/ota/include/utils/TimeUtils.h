/*
 * TimeUtils.h
 *
 *  Created on: 2014-7-19
 *      Author: manson
 */

#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include "utils/TimeUtils.h"

namespace utils {

class TimeUtils {
public:
	TimeUtils();
	virtual ~TimeUtils();

	static long GetTimeMicroSec();
};

}

#endif /* TIMEUTILS_H_ */

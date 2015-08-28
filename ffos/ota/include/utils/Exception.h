#ifndef CASTEXCEPTION_H_
#define CASTEXCEPTION_H_

#include <string>

#include "utils/Logger.h"

namespace utils {

class Exception {
private:
	std::string m_szException;

public:
	Exception(const std::string &e) {
		m_szException = e;
	}

	virtual ~Exception() {
	}

	void Print() {
		Log("%s", m_szException.c_str());
	}
};

}

#endif /* CASTEXCEPTION_H_ */

/*
 * Thread.h
 *
 *  Created on: 2014-7-23
 *      Author: manson
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

#include "utils/Exception.h"

namespace thread {

class Thread {
private:
	static void exit_handler(int sig);
	static void * runnable_proxy(void * arg);

	pthread_t m_ThreadId;
	bool m_AutoDelete;
	bool m_IsStarted;

protected:
	virtual void Run() = 0;

public:
	Thread(bool autoDelete);
	virtual ~Thread();

	int Start() throw (utils::Exception);
	int Stop(bool wait = true) throw (utils::Exception);
	int Join();
	bool IsAlive();

	//TODO:
//	int Join(int timeout/*microsecond*/);
};

}

#endif /* THREAD_H_ */

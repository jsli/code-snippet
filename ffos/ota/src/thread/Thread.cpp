/*
 * Thread.cpp
 *
 *  Created on: 2014-7-23
 *      Author: manson
 */

#include <signal.h>
#include <string.h>
#include <errno.h>

#include "thread/Thread.h"
#include "utils/Logger.h"

namespace thread {

const bool DEBUG = false;

Thread::Thread(bool autoDelete) {
	if (DEBUG) {
		Log("Thread[%ld] -------------- init!", m_ThreadId);
	}
	m_ThreadId = 0;
	m_AutoDelete = autoDelete;
	m_IsStarted = false;
}

Thread::~Thread() {
	if (DEBUG) {
		Log("Thread[%ld] -------------- gone!", m_ThreadId);
	}
}

void Thread::exit_handler(int signal) {
	if (DEBUG) {
		Log("exit_handler receiver signal : %d", signal);
	}

	if (signal == SIGUSR1) {
		pthread_exit(0);
	}
}

void * Thread::runnable_proxy(void * threadPtr) {
	if (threadPtr == NULL) {
		Log("runnable_proxy launch thread failed, arg is NULL");
		return NULL;
	}

	struct sigaction actions;
	memset(&actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = exit_handler;
	int rc = sigaction(SIGUSR1, &actions, NULL);
	if (rc != 0) {
		Log("runnable_proxy install exit signal action failed");
		return NULL;
	}

	Thread* thread = static_cast<Thread*>(threadPtr);
	thread->Run();
	if (thread->m_AutoDelete) {
		delete thread;
	}
	return NULL;
}

int Thread::Start() throw (utils::Exception) {
	if (m_IsStarted) {
		throw utils::Exception("Start can be called only once");
	}
	m_IsStarted = true;
	return pthread_create(&m_ThreadId, NULL, runnable_proxy, this);
}

int Thread::Stop(bool wait) throw (utils::Exception) {
	if (!m_IsStarted) {
		throw utils::Exception("Thread isn't running, cannot stop");
	}

	int result = pthread_kill(m_ThreadId, SIGUSR1);
	if (result != 0) {
		if (ESRCH == result) {
			Log("thread is already finished");
		} else {
			Log("kill thread failed, errno : %d", result);
		}
	} else { //send kill-signal successfully
		if (DEBUG) {
			Log("kill-signal send and join");
		}
		if (wait) {
			Join();
		}
	}
	if (DEBUG) {
		Log("join return");
	}
	return result;
}

int Thread::Join() {
	return pthread_join(m_ThreadId, NULL);
}

bool Thread::IsAlive() {
	int kill_rc = pthread_kill(m_ThreadId, 0);
	if (kill_rc == ESRCH) {
		if (DEBUG) {
			Log("thread[%ld] did not exists or already quit", m_ThreadId);
		}
		return false;
	}
	if (DEBUG) {
		Log("thread[%ld] is alive", m_ThreadId);
	}
	return true;
}

}

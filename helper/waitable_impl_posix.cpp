#include "waitable_impl_posix.h"

#include <errno.h>


WaitableImplPosix::WaitableImplPosix(){
	pthread_cond_init(&cond_, NULL);
}

WaitableImplPosix::~WaitableImplPosix() {
	pthread_cond_destroy(&cond_);
}

void WaitableImplPosix::Reset() {
}

void WaitableImplPosix::Signal() {
	pthread_cond_signal(&cond_);
}

void WaitableImplPosix::Broadcast() {
	pthread_cond_broadcast(&cond_);
}

int WaitableImplPosix::Wait(int milliseconds, WaitChecker* checker) {
	lock_.Acquire();
	if (!checker->wait()) {
		return 0;
	}
	bool wait_failed = false;
	int retval = -1;
	if (milliseconds < 0) {
		if (0 != pthread_cond_wait(&cond_, &lock_.mutex())) {
			wait_failed = true;
		}
		retval = wait_failed ? -1 : 0;
	} else {
		struct timespec abstime;
		abstime.tv_sec = (long)time(NULL) + milliseconds / 1000;
		abstime.tv_nsec = (milliseconds % 1000) * 1000000;
		int code = pthread_cond_timedwait(&cond_, &lock_.mutex(), &abstime);
		switch (code) {
		case 0:
			retval = 0;
			break;
		case ETIMEDOUT:
			retval = 1;
			break;
		default:
			wait_failed = true;
			retval = -1;
		}		
	}

	if (0 != retval) {
		lock_.Release();
	}
	return retval;
}

void WaitableImplPosix::Acquire() {
	lock_.Acquire();
}

void WaitableImplPosix::Release() {
	lock_.Release();
}


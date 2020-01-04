#pragma once

#include <pthread.h>
#include "waitable.h"
#include "lock_impl_posix.h"

class WaitableImplPosix : public IWaitable {
public:
	WaitableImplPosix();
	~WaitableImplPosix();

	void Reset();
	void Signal();
	void Broadcast();
	int Wait(int milliseconds, WaitChecker* checker);
	void Acquire();
	void Release();

	pthread_cond_t& cond() {
		return cond_;
	}

private:
	LockImplPosix lock_;
	pthread_cond_t cond_;
};


#pragma once

#include <pthread.h>
#include "lock.h"

class LockImplPosix : public ILock {
public:
	LockImplPosix();
	~LockImplPosix();
	bool Acquire();
	void Release();

	pthread_mutex_t& mutex() {
		return mutex_;
	}

private:
	pthread_mutex_t mutex_;
};

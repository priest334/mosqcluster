#include "lock_impl_posix.h"

LockImplPosix::LockImplPosix() {
	pthread_mutex_init(&mutex_, NULL);
}

LockImplPosix::~LockImplPosix() {
	pthread_mutex_destroy(&mutex_);
}

bool LockImplPosix::Acquire() {
	pthread_mutex_lock(&mutex_);
	return true;
}

void LockImplPosix::Release() {
	pthread_mutex_unlock(&mutex_);
}

#include "lock.h"

ILock::~ILock() {
}

AutoLock::AutoLock(ILock* lock) : lock_(lock) {
	if (lock_)
		lock_->Acquire();
}

AutoLock::~AutoLock() {
	if (lock_)
		lock_->Release();
}



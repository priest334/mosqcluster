#include "lock.h"

ILock::~ILock() {
}

AutoLock::AutoLock(ILock* lock) : lock_(lock) {
	lock_->Acquire();
}

AutoLock::~AutoLock() {
	lock_->Release();
}



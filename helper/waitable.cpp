#include "waitable.h"

#include "waitable_impl_posix.h"


IWaitable::~IWaitable() {
}

Waitable::Waitable() : waitable_(new WaitableImplPosix()), free_(1) {
}

Waitable::Waitable(IWaitable* waitable) : waitable_(waitable), free_(0) {
}

Waitable::~Waitable() {
	if (free_) {
		delete waitable_;
	}
}

void Waitable::Reset() {
	waitable_->Reset();
}

void Waitable::Signal() {
	waitable_->Signal();
}

void Waitable::Broadcast() {
	waitable_->Broadcast();
}

int Waitable::Wait(int milliseconds, WaitChecker* checker) {
	return waitable_->Wait(milliseconds, checker);
}

void Waitable::Acquire() {
	waitable_->Acquire();
}

void Waitable::Release() {
	waitable_->Release();
}



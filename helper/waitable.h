#pragma once

class WaitChecker {
public:
	virtual bool wait() = 0;
};

class IWaitable {
public:
	virtual ~IWaitable();
	virtual void Reset() = 0;
	virtual void Signal() = 0;
	virtual void Broadcast() = 0;
	virtual int Wait(int milliseconds, WaitChecker* checker) = 0;
	virtual void Acquire() = 0;
	virtual void Release() = 0;
};

class Waitable {
public:
	Waitable();
	Waitable(IWaitable* waitable);
	~Waitable();

	void Reset();
	void Signal();
	void Broadcast();
	int Wait(int milliseconds = -1, WaitChecker* checker = 0);
	void Acquire();
	void Release();

	
private:
	IWaitable* waitable_;
	int free_;
};


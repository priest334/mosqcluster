#pragma once


class ILock{
public:
	virtual ~ILock();
	virtual bool Acquire() = 0;
	virtual void Release() = 0;
};

class AutoLock {
private:
	AutoLock();
	AutoLock& operator=(const AutoLock& src);
public:
	AutoLock(ILock* lock);
	~AutoLock();

private:
	ILock* lock_;
};


#pragma once

#include <queue>
#include "waitable.h"

template <class _Type>
class TaskQueue : public WaitChecker {
public:
	TaskQueue() {
	}

	~TaskQueue() {
	}

	void Push(const _Type& value) {
		qlock_.Acquire();
		queue_.push(value);
		qlock_.Release();
		qlock_.Broadcast();
	}

	bool wait() {
		return queue_.empty();
	}

	bool Get(_Type& value) {
		bool retval = false;
		if (0 == qlock_.Wait(-1, this)) {
			if (!queue_.empty()) {
				value = queue_.front();
				queue_.pop();
				retval = true;
			}
			qlock_.Release();
		}
		return retval;
	}

private:
	Waitable qlock_;
	std::queue<_Type> queue_;
};


#pragma once

#include <pthread.h>
#include "task_queue_posix.h"


class ThreadPool {
public:
	class Task {
	public:
		virtual ~Task();
		virtual void Run() = 0;
		virtual void Finish();

		void Attach(ThreadPool* owner) {
			owner_ = owner;
		}
		ThreadPool* Owner() {
			return owner_;
		}
	private:
		ThreadPool* owner_;
	};

	ThreadPool();
	ThreadPool(int size);
	virtual ~ThreadPool();
	virtual void ThreadMain();

	void Create(int size);
	void Stop();
	void Wait();
	void Push(Task* task);

private:
	volatile int stop_;
	int size_;
	pthread_t* tids_;
	TaskQueue<Task*> qlist_;
};


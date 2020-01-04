#include "thread_pool.h"

namespace {
	void* ThreadProc(void* argp) {
		ThreadPool* p = reinterpret_cast<ThreadPool*>(argp);
		p->ThreadMain();
		return NULL;
	}
}

ThreadPool::Task::~Task() {
}

void ThreadPool::Task::Finish() {
}

ThreadPool::ThreadPool() {
}

ThreadPool::ThreadPool(int size) : stop_(true), size_(size), tids_(new pthread_t[size]) {
	Create(size_);
}

ThreadPool::~ThreadPool() {
	if (tids_) {
		delete[] tids_;
	}
}

void ThreadPool::ThreadMain() {
	while (!stop_) {
		Task* task = NULL;
		if (qlist_.Get(task)) {
			task->Run();
			task->Finish();
		}
	}
}

void ThreadPool::Create(int size) {
	stop_ = false;
	tids_ = new pthread_t[size];
	for (int i = 0; i < size; i++) {
		pthread_create(&tids_[i], NULL, ThreadProc, this);
	}
}

void ThreadPool::Stop() {
	stop_ = true;
}

void ThreadPool::Wait() {
	for (int i = 0; i < size_; i++) {
		pthread_join(tids_[i], NULL);
	}
}

void ThreadPool::Push(Task* task) {
	qlist_.Push(task);
}




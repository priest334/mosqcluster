#include "connection_pool.h"

#include "helper/lock_impl_posix.h"
#include "xdb.h"

namespace hlp {

	ConnectionPool::ConnectionPool() : current_size_(0), idle_size_(0), inc_size_(1), max_size_(-1), lock_(new LockImplPosix()){
	}

	ConnectionPool::~ConnectionPool() {
		delete lock_;
	}

	Connection* ConnectionPool::GetConnection() {
		AutoLock lock(lock_);
		Connection* conn = 0;
		if (idle_size_ == 0) {
			int size = 0;
			if (max_size_ < 0) {
				size = inc_size_;
			} else if (current_size_ < max_size_) {
				size = max_size_ - current_size_;
			}
			if (size > 0) {
				Create(size);
			}
		}
		conn = conn_list_.front();
		conn_list_.pop_front();
		idle_size_--;
		return conn;
	}

	void ConnectionPool::ReleaseConnection(Connection* conn) {
		AutoLock lock(lock_);
		conn_list_.push_back(conn);
		idle_size_++;
	}

	void ConnectionPool::Init(const string& dsn, int init_size/* = 5*/, int inc_size/* = 2*/, int max_size/* = -1*/) {
		dsn_ = dsn;
		inc_size_ = inc_size;
		max_size_ = max_size_;
		Create(init_size);
	}

	void ConnectionPool::Create(int size) {	
		for (int i = 0; i < size; i++) {
			Connection* conn = CreateConnection(dsn_);
			if (conn) {
				conn_list_.push_back(conn);
				idle_size_++;
				current_size_++;
			}
		}
	}

} // namespace hlp


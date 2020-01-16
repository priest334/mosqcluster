#include "connection_pool.h"

#include "helper/lock_impl_posix.h"
#include "xdb.h"

namespace hlp {

	ConnectionPool::ConnectionPool() {
	}

	ConnectionPool::ConnectionPool(const string& dsn) {
	}

	ConnectionPool::~ConnectionPool() {
	}

	Connection* ConnectionPool::Create() {
		return CreateConnection(dsn_);
	}

	void ConnectionPool::Destroy(Connection* conn) {
		ReleaseConnection(conn);
	}

	Connection& ConnectionPool::Get() {
		Connection& obj = pool_->GetObject();
		return obj;
	}

	void ConnectionPool::Release(Connection& conn) {
		pool_->ReleaseObject(conn);
	}

	void ConnectionPool::Init(const string& dsn, int init_size/* = 5*/, int inc_size/* = 2*/, int max_size/* = -1*/) {
		dsn_ = dsn;
		pool_ = new SimplePool<Connection, ConnectionPool>(*this, init_size, inc_size, max_size, new LockImplPosix());
	}

} // namespace hlp


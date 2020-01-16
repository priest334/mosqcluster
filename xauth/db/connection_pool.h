#pragma once

#include <list>
#include <string>

#include "helper/simple_pool.h"

namespace hlp {
	using std::list;
	using std::string;

	class Connection;	


	class ConnectionPool {
	public:
		ConnectionPool();
		ConnectionPool(const string& dsn);
		~ConnectionPool();

		Connection* Create();
		void Destroy(Connection* conn);

		Connection& Get();
		void Release(Connection& conn);

		void Init(const string& dsn, int init_size = 5, int inc_size = 2, int max_size = -1);
	private:
		string dsn_;
		SimplePool<Connection, ConnectionPool>* pool_;
	};
} // namespace hlp




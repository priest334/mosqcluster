#pragma once

#include <list>
#include <string>

#include "helper/lock.h"

namespace hlp {
	using std::list;
	using std::string;

	class Connection;	

	class ConnectionPool {
	public:
		ConnectionPool();
		~ConnectionPool();
		Connection* GetConnection();
		void ReleaseConnection(Connection* conn);

		void Init(const string& dsn, int init_size = 5, int inc_size = 2, int max_size = -1);

	protected:
		void Create(int size);
	private:
		int current_size_;
		int idle_size_;
		int inc_size_;
		int max_size_;
		ILock* lock_;
		string dsn_;
		list<Connection*> conn_list_;
	};
} // namespace hlp




#pragma once

#include <string>


namespace hlp {
	using std::string;

	class Connection;
	class ConnectionPool;
	class Statement;
	class ResultSet;


	class SqlWrapper {
	public:
		SqlWrapper(Connection& conn);
		SqlWrapper(Connection& conn, const string& sql);
		SqlWrapper(ConnectionPool* pool);
		SqlWrapper(ConnectionPool* pool, const string& sql);
		virtual ~SqlWrapper();

		Statement* Stmt() const;

		void SetSql(const string& sql);
		void Append(int type, const string& value);
		int Query(ResultSet* rs);
		int Update();

	private:
		ConnectionPool* pool_;
		Connection& conn_;
		Statement* stmt_;
		string sql_;
	};

} // namespace hlp


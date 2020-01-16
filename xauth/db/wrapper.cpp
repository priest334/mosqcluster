#include "wrapper.h"

#include "xdb.h"
#include "connection_pool.h"

namespace hlp {

	SqlWrapper::SqlWrapper(Connection& conn) : pool_(0), conn_(conn) {
		stmt_ = conn_.CreateStatement("");
	}

	SqlWrapper::SqlWrapper(Connection& conn, const string& sql) : pool_(0), conn_(conn), sql_(sql){
		stmt_ = conn_.CreateStatement(sql);
	}

	SqlWrapper::SqlWrapper(ConnectionPool* pool) : pool_(pool), conn_(pool_->Get()) {
		stmt_ = conn_.CreateStatement("");
	}

	SqlWrapper::SqlWrapper(ConnectionPool* pool, const string& sql) : pool_(pool), conn_(pool_->Get()), sql_(sql) {
		stmt_ = conn_.CreateStatement(sql);
	}

	SqlWrapper::~SqlWrapper() {
		if (stmt_)
			conn_.DestroyStatement(stmt_);
		if (pool_)
			pool_->Release(conn_);
	}

	Statement* SqlWrapper::Stmt() const {
		return stmt_;
	}

	void SqlWrapper::SetSql(const string& sql) {
		if (stmt_) {
			sql_ = sql;
			stmt_->SetSql(sql);
		}
	}

	void SqlWrapper::Append(int type, const string& value) {
		if (stmt_)
			stmt_->Append(type, value);
	}

	int SqlWrapper::Query(ResultSet* rs) {
		if (stmt_ && !sql_.empty())
			return conn_.Query(stmt_, rs);
		return 0;
	}

	int SqlWrapper::Update() {
		if (stmt_ && !sql_.empty())
			return conn_.Update(stmt_);
		return 0;
	}

} // namespace hlp



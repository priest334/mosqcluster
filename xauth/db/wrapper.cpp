#include "wrapper.h"

#include "xdb.h"
#include "connection_pool.h"

namespace hlp {

	ConnectionWrapper::ConnectionWrapper() : pool_(0), conn_(0) {
	}

	ConnectionWrapper::ConnectionWrapper(Connection* conn) : pool_(0), conn_(conn) {
	}

	ConnectionWrapper::ConnectionWrapper(ConnectionPool* pool) : pool_(pool) {
		conn_ = pool_->GetConnection();
	}

	ConnectionWrapper::~ConnectionWrapper() {
		if (pool_)
			pool_->ReleaseConnection(conn_);
	}

	Connection* ConnectionWrapper::operator->() {
		return conn_;
	}

	ConnectionWrapper::operator bool() const {
		return conn_ != 0;
	}

	SqlWrapper::SqlWrapper(Connection* conn) : conn_(conn){
		stmt_ = conn_->CreateStatement("");
	}

	SqlWrapper::SqlWrapper(Connection* conn, const string& sql) : conn_(conn), sql_(sql){
		stmt_ = conn_->CreateStatement(sql);
	}

	SqlWrapper::SqlWrapper(ConnectionPool* pool) : conn_(pool) {
		stmt_ = conn_->CreateStatement("");
	}

	SqlWrapper::SqlWrapper(ConnectionPool* pool, const string& sql) : conn_(pool), sql_(sql) {
		stmt_ = conn_->CreateStatement(sql);
	}

	SqlWrapper::~SqlWrapper() {
		if (stmt_)
			conn_->DestroyStatement(stmt_);
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
		if (conn_ && stmt_ && !sql_.empty())
			return conn_->Query(stmt_, rs);
		return 0;
	}

	int SqlWrapper::Update() {
		if (conn_ && stmt_ && !sql_.empty())
			return conn_->Update(stmt_);
		return 0;
	}

} // namespace hlp



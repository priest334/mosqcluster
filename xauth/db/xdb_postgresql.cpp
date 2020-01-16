#include "xdb_postgresql.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "global.h"


namespace hlp {
	using std::ostringstream;

	namespace {

		void PrintSQLException(const string& ex) {
			logger::Error() << ex.c_str();
		}
	}

	PostgresqlStatement::PostgresqlStatement() 
		: current_param_values_(default_param_values_) {
	}

	PostgresqlStatement::~PostgresqlStatement() {
		if (current_param_values_ != default_param_values_) {
			delete current_param_values_;
			current_param_values_ = NULL;
		}
	}

	int PostgresqlStatement::SetSql(const string& sql) {
		sql_ = sql;
		return 0;
	}

	int PostgresqlStatement::Append(const FieldType& type, const string& value) {
		ParamValue pv;
		pv.type_ = type;
		pv.value_ = value;
		param_values_.push_back(pv);
		if (values_.empty())
			values_ = value;
		else
			values_ += ", " + value;
		return 0;
	}

	string PostgresqlStatement::Values() {
		return values_;
	}

	PostgresqlStatement::ParamType* PostgresqlStatement::ParamValues() {
		size_t size = param_values_.size();
		current_param_values_ = default_param_values_;
		if (size > PG_STMT_PARAM_SIZE) {
			current_param_values_ = new const char*[size];
			memset(current_param_values_, 0, sizeof(current_param_values_[0])*size);
		}

		for (size_t i = 0; i < size; i++) {
			current_param_values_[i] = param_values_[i].value_.c_str();
		}
		return current_param_values_;
	}

	int PostgresqlStatement::ParamLengths() const {
		return values_.size();
	}

	PostgresqlConnection::PostgresqlConnection(PostgresqlConnectionFactory* factory)
		: factory_(factory),
		conn_(NULL) {
	}

	PostgresqlConnection::~PostgresqlConnection() {
		if (NULL != conn_) {
			PQfinish(conn_);
		}
	}

	int PostgresqlConnection::Create(DSN* dsn) {
		username_ = dsn->Username();
		password_ = dsn->Password();
		string dbname = dsn->DBName();
		if (username_.empty() || password_.empty() || dbname.empty())
			return -1;

		ostringstream ss;
		ss << "user=" << username_ << " ";
		ss << "password=" << password_ << " ";
		ss << "dbname=" << dbname << " ";
		if (!dsn->Host().empty())
			ss << "host=" << dsn->Host() << " ";
		if (!dsn->Port().empty())
			ss << "port=" << dsn->Port() << " ";

		//putenv("PGCLIENTENCODING=GBK");
		conn_ = PQconnectdb(ss.str().c_str());
		//string encstr = pg_encoding_to_char(PQclientEncoding(conn_));
		//PQsetClientEncoding(conn_, "GBK");
		//encstr = pg_encoding_to_char(PQclientEncoding(conn_));
		ConnStatusType status = PQstatus(conn_);
		if (CONNECTION_OK != status) {
			ss.clear();
			ss << status << ": " << PQerrorMessage(conn_) << std::endl;
			ss << "dsn: " << dsn->Str() << std::endl;
			PrintSQLException(ss.str());
			return -1;
		}
		return 0;
	}

	ConnectionFactory* PostgresqlConnection::Factory() const {
		return factory_;
	}

	Statement* PostgresqlConnection::CreateStatement(const string& sql) {
		PostgresqlStatement* stmt = new PostgresqlStatement();
		if (!conn_)
			return NULL;
		stmt->SetSql(sql);
		return stmt;
	}

	void PostgresqlConnection::DestroyStatement(Statement* stmt) {
		PostgresqlStatement* ostmt = dynamic_cast<PostgresqlStatement*>(stmt);
		delete ostmt;
	}

	RecordIndex PostgresqlConnection::Update(const string& sql) {
		RecordIndex retval = 0;
		PostgresqlStatement* stmt = dynamic_cast<PostgresqlStatement*>(CreateStatement(sql));
		retval = Update(stmt);
		return retval;
	}

	RecordIndex PostgresqlConnection::Update(Statement* update_statement) {
		if (NULL == update_statement)
			return 0;
		PostgresqlStatement* stmt = dynamic_cast<PostgresqlStatement*>(update_statement);
		RecordIndex retval = 0;

		PGresult* res = PQexecParams(conn_, stmt->sql_.c_str(), stmt->ParamLengths(), NULL, stmt->ParamValues(), NULL, NULL, 0);
		ExecStatusType status = PQresultStatus(res);
		switch (status)
		{
		case PGRES_EMPTY_QUERY:
		case PGRES_COMMAND_OK:
			break;
		case PGRES_BAD_RESPONSE:
		case PGRES_FATAL_ERROR:
			PQreset(conn_);
		default:
		{
			ostringstream ss;
			ss << status << ": " << PQerrorMessage(conn_) << std::endl;
			ss << "sql: " << stmt->sql_ << std::endl;
			if (!stmt->values_.empty())
				ss << stmt->values_ << std::endl;
			PrintSQLException(ss.str());
			retval = -1;
			break;
		}
		}

		PQclear(res);
		return retval;
	}

	RecordIndex PostgresqlConnection::Query(const string& sql, ResultSet* result) {
		RecordIndex retval = 0;
		PostgresqlStatement* stmt = dynamic_cast<PostgresqlStatement*>(CreateStatement(sql));
		retval = Query(stmt, result);
		return retval;
	}

	RecordIndex PostgresqlConnection::Query(Statement* query_statement, ResultSet* result) {
		if (NULL == query_statement)
			return 0;
		PostgresqlStatement* stmt = dynamic_cast<PostgresqlStatement*>(query_statement);
		RecordIndex retval = 0;
		PGresult* res = PQexecParams(conn_, stmt->sql_.c_str(), stmt->ParamLengths(), NULL, stmt->ParamValues(), NULL, NULL, 0);
		ExecStatusType status = PQresultStatus(res);
		switch (status)
		{
		case PGRES_EMPTY_QUERY:
		case PGRES_COMMAND_OK:
			break;
		case PGRES_TUPLES_OK:
		{
			int rows = PQntuples(res);
			retval = rows;
			if (NULL == result)
				break;
			int cols = PQnfields(res);
			result->SetFieldSize(cols);
			for (int index = 0; index < cols; index++) {
				Oid type = PQftype(res, index);
				string name = PQfname(res, index);
				result->SetType(index, FieldType(type));
				result->SetName(index, name);
			}
			for (int i = 0; i < rows; i++) {
				ResultRow* row = ResultSet::New();
				for (int j = 0; j < cols; j++) {
					ResultField* field = ResultRow::New();
					if (PQgetisnull(res, i, j)) {
						field->SetNull();
						row->Append(field);
						continue;
					}
					field->SetValue(PQgetvalue(res, i, j));
					row->Append(field);
				}
				result->Append(row);
			}
		}
		break;
		case PGRES_BAD_RESPONSE:
		case PGRES_FATAL_ERROR:
			PQreset(conn_);
		default:
		{
			ostringstream ss;
			ss << status << ": " << PQerrorMessage(conn_) << std::endl;
			ss << "sql: " << stmt->sql_ << std::endl;
			if (!stmt->values_.empty())
				ss << stmt->values_ << std::endl;
			PrintSQLException(ss.str());
			retval = -1;
			break;
		}
		break;
		}

		PQclear(res);

		return retval;
	}

	bool PostgresqlConnection::IsValidate() {
		if (NULL == conn_)
			return false;
		return (CONNECTION_OK == PQstatus(conn_));
	}

	bool PostgresqlConnection::Reconnect() {
		if (NULL == conn_)
			return false;
		PQreset(conn_);
		return (CONNECTION_OK == PQstatus(conn_));
	}


	Connection* PostgresqlConnectionFactory::Create(DSN* dsn) {
		PostgresqlConnection* conn = new PostgresqlConnection(this);
		conn->Create(dsn);
		return conn;
	}

	void PostgresqlConnectionFactory::Destroy(Connection* conn) {
		PostgresqlConnection* dconn = dynamic_cast<PostgresqlConnection*>(conn);
		delete dconn;
	}

	bool PostgresqlConnectionFactory::IsSupported(const string& name) {
		return (name == "postgres" || name == "postgresql" || name == "pg");
	}

} // namespace hlp




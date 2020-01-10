

#ifndef __XDB_POSTGRESQL_H__
#define __XDB_POSTGRESQL_H__

#include <vector>
#include <libpq-fe.h>
#include "xdb.h"


#define PG_STMT_PARAM_SIZE 32

namespace hlp {
	using std::vector;

	class PostgresqlConnection;

	class PostgresqlStatement : public Statement {
		friend class PostgresqlConnection;
		typedef struct param_value_s {
			FieldType type_;
			string value_;
		}ParamValue;
		typedef vector<ParamValue> ParamValueList;
		typedef const char* ParamType;
	public:
		PostgresqlStatement();
		~PostgresqlStatement();
		int SetSql(const string& sql);
		int Append(const FieldType& type, const string& value);
		string Values();
		ParamType* ParamValues();
		int ParamLengths() const;
	private:
		string sql_;
		string values_;
		ParamValueList param_values_;
		ParamType default_param_values_[PG_STMT_PARAM_SIZE];
		ParamType* current_param_values_;
	};

	class PostgresqlConnectionFactory;
	class PostgresqlConnection : public Connection {
	public:
		PostgresqlConnection(PostgresqlConnectionFactory* factory);
		~PostgresqlConnection();
		int Create(DSN* connstr);
	public:
		ConnectionFactory* Factory() const;
		Statement* CreateStatement(const string& sql);
		void DestroyStatement(Statement* stmt);
		RecordIndex Update(const string& sql);
		RecordIndex Update(Statement* update_statement);
		RecordIndex Query(const string& sql, ResultSet* result);
		RecordIndex Query(Statement* query_statement, ResultSet* result);

		bool IsValidate();
		bool Reconnect();

	private:
		PostgresqlConnectionFactory* factory_;
		PGconn* conn_;
		string username_;
		string password_;
		string connstr_;
	};

	class PostgresqlConnectionFactory : public ConnectionFactory {
	public:
		Connection* Create(DSN* dsn);
		void Destroy(Connection* conn);
		bool IsSupported(const string& name);
	};
} // namespace hlp

#endif // __XDB_POSTGRESQL_H__



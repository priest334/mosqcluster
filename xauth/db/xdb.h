#ifndef __XDB_H__
#define __XDB_H__

#include <string>
#include <vector>
#include <map>

#include "record.h"


namespace hlp {
	using std::string;
	using std::vector;
	using std::map;

	typedef int RecordIndex;
	typedef int FieldType;

	extern const FieldType kFieldTypeNone;
	extern const FieldType kFieldTypeNumeric;
	extern const FieldType kFieldTypeText;
	extern const FieldType kFieldDateTime;

	class DSN {
	public:
		DSN();
		DSN(const string& str);
		void Clear();
		int Parse(const string& str);
		string Str() const;
		string Type() const;
		string Host() const;
		string Port() const;
		string DBName() const;
		string Username() const;
		string Password() const;
		string Param(const string& key) const;
	private:
		string connstr_;
		string dbtype_;
		string host_;
		string port_;
		string dbname_;
		string username_;
		string password_;
		map<string, string> parameters_;
	};

	class ResultField {
	public:
		ResultField();
		~ResultField();
		void SetValue(const string& value);
		string Value() const;
		void SetNull();
		bool IsNull() const;
	private:
		string value_;
		bool null_;
	};

	class ResultFieldTypeName {
	public:
		ResultFieldTypeName();
		~ResultFieldTypeName();
		void SetType(FieldType type);
		void SetName(const string& name);
		string Name() const;
	private:
		FieldType type_;
		string name_;
	};

	class ResultRow : public Record<ResultField> {
	public:
	};

	class ResultSet : public Record<ResultRow> {
	public:
		void SetFieldSize(int size);
		void SetType(int index, FieldType type);
		void SetName(int index, const string& name);
		int FieldSize() const;
		string Name(int index);
		int Index(const string& name);
	private:
		vector<ResultFieldTypeName> field_info_;
	};

	class Statement {
	public:
		virtual ~Statement();
		virtual int SetSql(const string& sql) = 0;
		virtual int Append(const FieldType& type, const string& value) = 0;
		virtual string Values() = 0;
	private:
		string sql_;
	};

	class ConnectionFactory;
	class Connection {
	public:
		virtual ~Connection();
		virtual ConnectionFactory* Factory() const = 0;
		virtual Statement* CreateStatement(const string& sql) = 0;
		virtual void DestroyStatement(Statement* stmt) = 0;
		virtual RecordIndex Update(const string& sql) = 0;
		virtual RecordIndex Update(Statement* update_statement) = 0;
		virtual RecordIndex Query(const string& sql, ResultSet* result) = 0;
		virtual RecordIndex Query(Statement* query_statement, ResultSet* result) = 0;
		virtual bool IsValidate() = 0;
		virtual bool Reconnect() = 0;
	};

	class ConnectionFactory {
	public:
		virtual ~ConnectionFactory();
		virtual Connection* Create(DSN* dsn) = 0;
		virtual void Destroy(Connection* conn) = 0;
		virtual bool IsSupported(const string& name) = 0;
	};


	Connection* CreateConnection(const string& str);
	void ReleaseConnection(Connection* conn);

} // namespace hlp
#endif // __XDB_H__


#include "xdb.h"

#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <sstream>
#include "xdb_postgresql.h"


namespace hlp
{
	namespace {
		string ToUpper(const string& str) {
			string retval;
			retval.resize(str.length());
			string::size_type index, len = str.length();
			for (index = 0; index < len; index++)
				retval[index] = toupper(str[index]);
			return retval;
		}

		string ToLower(const string& str) {
			string retval;
			retval.resize(str.length());
			string::size_type index, len = str.length();
			for (index = 0; index < len; index++)
				retval[index] = tolower(str[index]);
			return retval;
		}
	}
	const char* kHost = "host";
	const char* kPort = "port";
	const char* kDBName = "dbname";
	const char* kUserName = "user";
	const char* kPassword = "password";
	const char* kMaxConn = "maxconn";
	const char* kMinConn = "minconn";
	const char* kIncConn = "incrconn";
	const char* kConnTimeout = "connect_timeout";
	const char* kClientEncoding = "client_encoding";
	const char* kPrefixOracle = "oracle";
	const char* kPrefixPostgresql = "postgresql";

	const FieldType kFieldTypeNone = 0;
	const FieldType kFieldTypeNumeric = 1;
	const FieldType kFieldTypeText = 2;
	const FieldType kFieldDateTime = 3;


	DSN::DSN() {
	}

	DSN::DSN(const string& str) {
		Parse(str);
	}

	void DSN::Clear() {
		connstr_.clear();
		dbtype_.clear();
		host_.clear();
		port_.clear();
		dbname_.clear();
		username_.clear();
		password_.clear();
		parameters_.clear();
	}

	//         1                                    2        3
	// [dbtype]://[user[:password]@][host][:port][/dbname][?param1=value1&...]
	int DSN::Parse(const string& str) {
		Clear();
		connstr_ = str;
		string::size_type fpos, fpos1, fpos2, fpos3, offset, length = str.length();
		fpos1 = str.find("://");
		if (fpos1 == string::npos)
			return -1;

		dbtype_ = str.substr(0, fpos1);
		offset = fpos1 + 3;
		fpos2 = str.find('/', offset);
		if (fpos2 == string::npos)
			fpos2 = length;

		fpos = str.find("@", offset);
		if (fpos != string::npos) {
			string::size_type ftmp = str.find(":", offset);
			if (ftmp != string::npos && ftmp < fpos) {
				username_ = str.substr(offset, ftmp - offset);
				password_ = str.substr(ftmp + 1, fpos - ftmp - 1);
			}
			else {
				username_ = str.substr(offset, fpos - offset);
			}
			offset = fpos + 1;
		}

		fpos = str.find(":", offset);
		if (fpos != string::npos) {
			host_ = str.substr(offset, fpos - offset);
			port_ = str.substr(fpos + 1, fpos2 - fpos - 1);
		}
		else {
			host_ = str.substr(offset, fpos2 - offset);
		}
		if (fpos2 >= length)
			return 0;

		offset = fpos2 + 1;

		fpos3 = str.find('?', offset);
		if (fpos3 == string::npos) {
			dbname_ = str.substr(offset);
		}
		else {
			dbname_ = str.substr(offset, fpos3 - offset);
			offset = fpos3 + 1;
			bool exit = false;
			while (!exit) {
				fpos = str.find('&', offset);
				if (fpos == string::npos) {
					fpos = length;
					exit = true;
				}
				string::size_type ftmp = str.find("=", offset);
				if (ftmp != string::npos) {
					string key = str.substr(offset, ftmp - offset);
					string value = str.substr(ftmp + 1, fpos - ftmp - 1);
					if (!key.empty())
						parameters_[key] = value;
				}
				offset = fpos + 1;
			}
		}
		return 0;
	}

	string DSN::Str() const { return connstr_; }
	string DSN::Type() const { return dbtype_; }
	string DSN::Host() const { return host_.empty() ? Param(kHost) : host_; }
	string DSN::Port() const { return port_.empty() ? Param(kPort) : port_; }
	string DSN::DBName() const { return dbname_.empty() ? Param(kDBName) : dbname_; }
	string DSN::Username() const { return username_.empty() ? Param(kUserName) : username_; }
	string DSN::Password() const { return password_.empty() ? Param(kPassword) : password_; }
	string DSN::Param(const string& key) const {
		map<string, string>::const_iterator iter = parameters_.find(key);
		return (parameters_.end() != iter) ? iter->second : string();
	}

	ResultField::ResultField() : null_(false) {
	}

	ResultField::~ResultField() {
	}

	void ResultField::SetValue(const string& value) {
		value_ = value;
	}

	string ResultField::Value() const {
		return value_;
	}

	void ResultField::SetNull() {
		null_ = true;
	}

	bool ResultField::IsNull() const {
		return null_;
	}

	ResultFieldTypeName::ResultFieldTypeName() {
	}

	ResultFieldTypeName::~ResultFieldTypeName() {
	}

	void ResultFieldTypeName::SetType(FieldType type) {
		type_ = type;
	}

	void ResultFieldTypeName::SetName(const string& name) {
		name_ = ToUpper(name);
	}

	string ResultFieldTypeName::Name() const {
		return name_;
	}

	void ResultSet::SetFieldSize(int size) {
		if (size <= 0)
			return;
		field_info_.resize(size);
	}

	void ResultSet::SetType(int index, FieldType type) {
		field_info_[index].SetType(type);
	}

	void ResultSet::SetName(int index, const string& name) {
		field_info_[index].SetName(name);
	}

	int ResultSet::FieldSize() const {
		return field_info_.size();
	}

	string ResultSet::Name(int index) {
		return field_info_[index].Name();
	}

	int ResultSet::Index(const string& name) {
		string field_name = ToUpper(name);
		int size = field_info_.size();
		for (int i = 0; i < size; i++) {
			if (field_info_[i].Name() == field_name)
				return i;
		}
		return -1;
	}

	Statement::~Statement() {

	}

	Connection::~Connection() {

	}

	ConnectionFactory::~ConnectionFactory() {

	}
		
	namespace {

		ConnectionFactory* factory_table[] = {
			new PostgresqlConnectionFactory()
		};

		ConnectionFactory* Factory(const string& type) {
			size_t count = sizeof(factory_table) / sizeof(factory_table[0]);
			for (size_t i = 0; i < count; i++) {
				if (factory_table[i]->IsSupported(type)) {
					return factory_table[i];
				}
			}
			return 0;
		}
	}

	Connection* CreateConnection(const string& str) {
		DSN dsn(str);
		string prefix = dsn.Type();
		ConnectionFactory* factory = Factory(dsn.Type());
		if (factory) {
			return factory->Create(&dsn);
		}
		return 0;
	}

	void ReleaseConnection(Connection* conn) {
		if (conn && conn->Factory())
			conn->Factory()->Destroy(conn);
	}

} // namespace hlp




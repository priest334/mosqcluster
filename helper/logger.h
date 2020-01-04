#pragma once

#include <memory.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <fstream>


typedef std::ostringstream OStrStream;
namespace logger {
	typedef enum {
		Debug = 0,
		Info,
		Warning,
		Error,
		Fatal,
		MaxLevel
	}LogLevel;
} // namespace logger


class ILock;

class Storage {
public:
	virtual ~Storage();
	virtual void Write(const std::string& message) = 0;
};

class Console : public Storage {
public:
	Console(ILock* lock = NULL);
	void Write(const std::string& message);

private:
	ILock* lock_;
};

class LogFile : public Storage {
public:
	LogFile(const std::string& filename, ILock* lock = NULL);
	void Write(const std::string& message);

private:
	ILock* lock_;
	std::ofstream ofs_;
};

class Message {
public:
	Message(logger::LogLevel level);
	~Message();

	template<class _Type>
	Message& operator<<(const _Type& value) {
		oss_ << value;
		return *this;
	}

	Message& format(const char* fmt, ...);
	std::string const str() const;

private:
	logger::LogLevel level_;
	OStrStream oss_;
};

class Logger {
public:
	typedef void (Logger::*LogMethod)(const Message&);

	Logger(logger::LogLevel level = logger::Debug);
	~Logger();

	void SetLevel(logger::LogLevel level);
	void SetStorage(Storage* storage);

	void debug(const Message& message);
	void info(const Message& message);
	void warning(const Message& message);
	void error(const Message& message);
	void fatal(const Message& message);

	void log(logger::LogLevel level, const Message& message);

	static void Initialize();
	static void CleanUp();
	static Logger* Get();

private:
	logger::LogLevel level_;
	Storage* storage_;
	static Logger* logger_;
};



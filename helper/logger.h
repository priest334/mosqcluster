#pragma once

#include <memory.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <fstream>


typedef std::ostringstream OStrStream;
class ILock;

namespace hlp {
	typedef enum {
		Debug = 0,
		Info,
		Warning,
		Error,
		Fatal,
		MaxLevel
	}LogLevel;

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

	class SingleFile : public Storage {
	public:
		SingleFile(const std::string& filename, ILock* lock = NULL);
		void Write(const std::string& message);

	private:
		ILock* lock_;
		std::ofstream ofs_;
	};

	class Logger;

	class Message : public OStrStream {
	public:
		Message(LogLevel level, Logger* logger = 0);
		virtual ~Message();

		Message& format(const char* fmt, ...);

	private:
		LogLevel level_;
		Logger* logger_;
	};

	class Logger {
	public:
		typedef void (Logger::*LogMethod)(const Message&);

		Logger(LogLevel level = Debug);
		~Logger();

		void SetLevel(LogLevel level);
		void SetStorage(Storage* storage);

		void debug(const Message& message);
		void info(const Message& message);
		void warning(const Message& message);
		void error(const Message& message);
		void fatal(const Message& message);

		void log(LogLevel level, const Message& message);
	private:
		LogLevel level_;
		Console* console_;
		Storage* storage_;
	};

} // namespace logger


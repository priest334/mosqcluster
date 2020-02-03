#include "logger.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "xtime.h"
#include "lock.h"


#ifdef _WIN32
#define ThreadVariable __declspec(thread)
#else
#define ThreadVariable __thread
#endif


ThreadVariable char* message_buffer = 0;
ThreadVariable int message_buffer_size = 4 * 1024;


namespace hlp {

	const char* LogLevelName[MaxLevel] = {
		"Debug",
		"Info",
		"Warning",
		"Error",
		"Fatal"
	};

	Storage::~Storage() {
	}

	Console::Console(ILock* lock) : lock_(lock) {
	}

	void Console::Write(const std::string& message) {
		if (lock_) {
			AutoLock lock(lock_);
			std::cout << message << std::endl;
		} else {
			std::cout << message << std::endl;
		}
	}

	SingleFile::SingleFile(const std::string& filename, ILock* lock) : lock_(lock) {
		ofs_.open(filename.c_str(), std::ios_base::app);
	}

	void SingleFile::Write(const std::string& message) {
		if (lock_) {
			AutoLock lock(lock_);
			ofs_ << message << std::endl;
		} else {
			ofs_ << message << std::endl;
		}
	}

	Message::Message(LogLevel level, Logger* logger/* = 0*/) : level_(level), logger_(logger) {
		(*this) << "[" << hlp::Time::Now().ToString() << "]-" << LogLevelName[level] << " ";
	}

	Message::~Message() {
		if (logger_)
			logger_->log(level_, *this);
	}

	Message& Message::format(const char* fmt, ...) {
		if (0 == message_buffer) {
			message_buffer = (char*)malloc(message_buffer_size);
		}
		if (message_buffer) {
			va_list args;
			va_start(args, fmt);
			int wr = vsnprintf(message_buffer, message_buffer_size, fmt, args);
			if (wr > 0) {
				if (wr >= message_buffer_size) {
					*(message_buffer + message_buffer_size - 32) = '\0';
					int more = wr - message_buffer_size + 32;
					(*this) << message_buffer << "..." << more << "more bytes";
				} else {
					(*this) << message_buffer;
				}
			}
			va_end(args);
		}
		return *this;
	}


	Logger::Logger(LogLevel level/* = logger::Debug*/) : level_(level), console_(new Console()), storage_(console_) {
	}


	Logger::~Logger() {
		delete console_;
	}


	void Logger::SetLevel(LogLevel level) {
		level_ = level;
	}

	void Logger::SetStorage(Storage* storage) {
		storage_ = storage;
	}

	void Logger::debug(const Message& message) {
		if (Debug >= level_) {
			storage_->Write(message.str());
		}
	}

	void Logger::info(const Message& message) {
		if (Info >= level_) {
			storage_->Write(message.str());
		}
	}

	void Logger::warning(const Message& message) {
		if (Warning >= level_) {
			storage_->Write(message.str());
		}
	}

	void Logger::error(const Message& message) {
		if (Error >= level_) {
			storage_->Write(message.str());
		}
	}

	void Logger::fatal(const Message& message) {
		if (Fatal >= level_) {
			storage_->Write(message.str());
		}
	}

	void Logger::log(LogLevel level, const Message& message) {
		LogMethod method[MaxLevel] = {
			&Logger::debug,
			&Logger::info,
			&Logger::warning,
			&Logger::error,
			&Logger::fatal
		};
		if (level_ >= Debug && level_ < MaxLevel) {
			(this->*method[level_])(message);
		}
	}
} // namespace logger



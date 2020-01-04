#include "logger.h"

#include <stdlib.h>
#include <iostream>
#include "xtime.h"
#include "lock.h"


#ifdef _WIN32
#define ThreadVariable __declspec(thread)
#else
#define ThreadVariable __thread
#endif

static const char* LogLevelName[logger::MaxLevel] = {
	"Debug",
	"Info",
	"Warning",
	"Error",
	"Fatal"
};

ThreadVariable char* message_buffer = 0;
ThreadVariable int message_buffer_size = 4 * 1024;

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

LogFile::LogFile(const std::string& filename, ILock* lock) : lock_(lock) {
	ofs_.open(filename.c_str(), std::ios_base::app);
}

void LogFile::Write(const std::string& message) {
	if (lock_) {
		AutoLock lock(lock_);
		ofs_ << message << std::endl;
	} else {
		ofs_ << message << std::endl;
	}
}


Message::Message(logger::LogLevel level) : level_(level) {
	oss_ << "[" << hlp::Time::Now().ToString() << "]-" << LogLevelName[level] << " ";
}

Message::~Message() {
	Logger* logger = Logger::Get();
	logger->log(level_, *this);
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
				oss_ << message_buffer << "..." << more << "more bytes";
			}
			else {
				oss_ << message_buffer;
			}
		}
		va_end(args);
	}
	return *this;
}

std::string const Message::str() const {
	return oss_.str();
}


Logger::Logger(logger::LogLevel level/* = Debug*/) : level_(level), storage_(0) {
}


Logger::~Logger() {
}


void Logger::SetLevel(logger::LogLevel level) {
	level_ = level;
}

void Logger::SetStorage(Storage* storage) {
	storage_ = storage;
}

void Logger::debug(const Message& message) {
	if (logger::Debug >= level_) {
		storage_->Write(message.str());
	}
}

void Logger::info(const Message& message) {
	if (logger::Info >= level_) {
		storage_->Write(message.str());
	}
}

void Logger::warning(const Message& message) {
	if (logger::Warning >= level_) {
		storage_->Write(message.str());
	}
}

void Logger::error(const Message& message) {
	if (logger::Error >= level_) {
		storage_->Write(message.str());
	}
}

void Logger::fatal(const Message& message) {
	if (logger::Fatal >= level_) {
		storage_->Write(message.str());
	}
}

void Logger::log(logger::LogLevel level, const Message& message) {
	LogMethod method[logger::MaxLevel] = {
		&Logger::debug,
		&Logger::info,
		&Logger::warning,
		&Logger::error,
		&Logger::fatal
	};
	if (level_ >= logger::Debug && level_ < logger::MaxLevel) {
		(this->*method[level_])(message);
	}
}

Logger* Logger::logger_ = NULL;

void Logger::Initialize() {
	logger_ = new Logger();
}

void Logger::CleanUp() {
	delete logger_;
}

Logger* Logger::Get() {
	return logger_;
}




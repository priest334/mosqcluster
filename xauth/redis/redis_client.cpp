#include "redis_client.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined(_WIN32)
#include <sys/timeb.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif
#include "hiredis/hiredis.h"
#include "helper/string_helper.h"
#include "global.h"

#ifndef unused_vairable
#define unused_vairable(x)
#endif

namespace {
	class RedisCommand {
	public:
		RedisCommand(redisContext* c, const char* fmt, ...) : reply_(NULL) {
			va_list args;
			va_start(args, fmt);
			reply_ = (redisReply*)redisvCommand(c, fmt, args);
			va_end(args);
		}
		~RedisCommand() {
			if (reply_) {
				freeReplyObject(reply_);
			}
		}

		bool IsOK() const {
			if (reply_ && reply_->type == REDIS_REPLY_STATUS) {
				return strcmp(reply_->str, "OK");
			}
			return false;
		}

		string str() const {
			if (reply_ && reply_->type == REDIS_REPLY_STRING) {
				return string(reply_->str);
			}
			return string();
		}

		long long integer() const {
			if (reply_ && reply_->type == REDIS_REPLY_INTEGER) {
				return reply_->integer;
			}
			return -1;
		}
	private:
		redisReply* reply_;
	};
}


namespace hlp {
	RedisClient::RedisClient() : context_(NULL) {
	}

	RedisClient::RedisClient(const string& server) : context_(NULL) {
		Connect(server);
	}

	RedisClient::~RedisClient() {
		if (context_)
			redisFree(context_);
	}

	int RedisClient::Connect(const string& server) {
		hlp::StringVector v(server, ":");
		string ip = v[0].empty() ? "127.0.0.1" : v[0];
		int port = v[1].empty() ? 6379 : atoi(v[1].c_str());
		struct timeval timeout = { 1, 500000 }; // 1.5 seconds
		context_ = redisConnectWithTimeout(ip.c_str(), port, timeout);
		if (context_ == NULL || context_->err) {
			if (context_) {
				logger::Error() << "Connection error: " << context_->errstr;
				redisFree(context_);
			} else {
				logger::Error() << "Connection error: can't allocate redis context";
			}
			return -1;
		}
		redisEnableKeepAlive(context_);
		redisSetTimeout(context_, timeout);
		return 0;
	}

	void RedisClient::Reconnect() {
		redisFree(context_);
		Connect(server_);
	}

	int RedisClient::Auth(const string& password) {	
		return RedisCommand(context_, "AUTH %s", password.c_str()).IsOK();
	}

	void RedisClient::Set(const string& key, const string& value, time_t expires_in) {
		RedisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
		RedisCommand(context_, "EXPIRE %s %d", key.c_str(), expires_in);	
	}

	string RedisClient::Get(const string& key, bool cleanup) {
		unused_vairable(cleanup);
		return RedisCommand(context_, "GET %s", key.c_str()).str();
	}


} // namespace hlp


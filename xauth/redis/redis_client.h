#pragma once


#include <time.h>
#include <string>

struct redisContext;

namespace hlp {
	using std::string;


	class RedisClient {
	public:
		RedisClient();
		RedisClient(const string& server);
		~RedisClient();

		int Connect(const string& server);
		void Reconnect();
		int Auth(const string& password);

		void Set(const string& key, const string& value, time_t expires_in);
		string Get(const string& key, bool cleanup);
	private:
		redisContext* context_;
		string server_;
	};

} // namespace hlp





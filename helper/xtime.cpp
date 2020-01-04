#include "xtime.h"

#include <time.h>
#if defined(_WIN32)
#include <sys/timeb.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif

namespace hlp {
	Time Time::Now() {
		return Time();
	}

	int Time::GetTimeOfDay(struct timeval* tv, void* tz) {
#if defined(_WIN32)
		struct tm t;
		SYSTEMTIME st;
		GetLocalTime(&st);
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_hour = st.wHour;
		t.tm_min = st.wMinute;
		t.tm_sec = st.wSecond;
		t.tm_isdst = -1;

		tv->tv_sec = mktime(&t);
		tv->tv_usec = st.wMilliseconds * 1000;
		return 0;
#else
		return gettimeofday(tv, (struct timezone*)tz);
#endif
	}

	DWORD Time::GetTickCount() {
#if defined(_WIN32)
		return ::GetTickCount();
#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
	}

	Time::Time()
		: timestamp_(time(NULL)),
		ftimestamp_(0) {
		struct timeval tv;
		Time::GetTimeOfDay(&tv, NULL);
		ftimestamp_ = tv.tv_usec;
	}

	Time::Time(time_t t)
		: timestamp_(t),
		ftimestamp_(0) {
	}

	std::string Time::Format(const char* fmt) {
		struct tm t;
		LocalTime(&t, &timestamp_);
		const int size = 64;
		char prefix[size] = { 0 };
		strftime(prefix, size, fmt, &t);
		return std::string(prefix);
	}

	std::string Time::ToString() {
		return Format("%Y-%m-%d %H:%M:%S");
	}

	time_t Time::Float() const {
		return ftimestamp_;
	}
} // namespace hlp



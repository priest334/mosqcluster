#pragma once

#ifndef __BASE_TIME_TIME_H__
#define __BASE_TIME_TIME_H__

#if defined(_WIN32)
#include <time.h>
#define LocalTime(t, r) localtime_s(t, r)
#else // OS_WIN
#include <time.h>
#include <sys/time.h>
#	define LocalTime(t, r) localtime_r(r, t)
#endif

#include <string>


#ifndef DWORD
#define DWORD unsigned long long
#endif


struct timeval;

namespace hlp {
	class Time {
	public:
		Time();
		Time(time_t t);

	public:
		static Time Now();
		static int GetTimeOfDay(struct timeval* tv, void* tz);
		static DWORD GetTickCount();

		std::string Format(const char* fmt);
		std::string ToString();
		time_t Float() const;
		time_t Timestamp() const;

	private:
		time_t timestamp_;
		time_t ftimestamp_;
	};
} // namespace hlp

#endif // __BASE_TIME_TIME_H__


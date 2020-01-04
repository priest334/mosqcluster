
#include "string_convert.h"

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "string_helper.h"


namespace hlp {
	namespace {

#define NONE_INPUT 0
#define INVALID_INPUT -1
#define SMALL_OUTPUT -2

		int WcharFromUtf8(const unsigned char* utf8, int size, wchar_t& wch) {
			if (!utf8 || !size)
				return NONE_INPUT;

			unsigned char ch = utf8[0];
				
			if (ch < 0x80) {
				wch = ch;
				return 1;
			}
			else if (ch < 0xc0) {
				if (size < 2 || !((utf8[1] ^ 0x80) < 0x40))
					return INVALID_INPUT;
				wch = ((wchar_t)(ch & 0x1f) << 6) | ((wchar_t)(utf8[1] ^ 0x80));
				return 2;
			}
			else if (ch < 0xf0) {
				if (size < 3)
					return INVALID_INPUT;
				if (!((utf8[1] ^ 0x80) < 0x40 && (utf8[2] ^ 0x80) < 0x40
					&& (ch >= 0xe1 || utf8[1] >= 0xa0)))
					return INVALID_INPUT;
				wch = ((wchar_t)(ch & 0x0f) << 12)
					| ((wchar_t)(utf8[1] ^ 0x80) << 6)
					| (wchar_t)(utf8[2] ^ 0x80);
				return 3;
			}
			else if (ch < 0xf8) {
				if (size < 4)
					return INVALID_INPUT;
				if (!((utf8[1] ^ 0x80) < 0x40 && (utf8[2] ^ 0x80) < 0x40
					&& (utf8[3] ^ 0x80) < 0x40
					&& (ch >= 0xf1 || utf8[1] >= 0x90)))
					return INVALID_INPUT;
				wch = ((wchar_t)(ch & 0x07) << 18)
					| ((wchar_t)(utf8[1] ^ 0x80) << 12)
					| ((wchar_t)(utf8[2] ^ 0x80) << 6)
					| (wchar_t)(utf8[3] ^ 0x80);
				return 4;
			}
			else if (ch < 0xfc) {
				if (size < 5)
					return INVALID_INPUT;
				if (!((utf8[1] ^ 0x80) < 0x40 && (utf8[2] ^ 0x80) < 0x40
					&& (utf8[3] ^ 0x80) < 0x40 && (utf8[4] ^ 0x80) < 0x40
					&& (ch >= 0xf9 || utf8[1] >= 0x88)))
					return INVALID_INPUT;
				wch = ((wchar_t)(size & 0x03) << 24)
					| ((wchar_t)(utf8[1] ^ 0x80) << 18)
					| ((wchar_t)(utf8[2] ^ 0x80) << 12)
					| ((wchar_t)(utf8[3] ^ 0x80) << 6)
					| (wchar_t)(utf8[4] ^ 0x80);
				return 5;
			}
			else if (ch < 0xfe) {
				if (size < 6)
					return INVALID_INPUT;
				if (!((utf8[1] ^ 0x80) < 0x40 && (utf8[2] ^ 0x80) < 0x40
					&& (utf8[3] ^ 0x80) < 0x40 && (utf8[4] ^ 0x80) < 0x40
					&& (utf8[5] ^ 0x80) < 0x40
					&& (ch >= 0xfd || utf8[1] >= 0x84)))
					return INVALID_INPUT;
				wch = ((wchar_t)(ch & 0x01) << 30)
					| ((wchar_t)(utf8[1] ^ 0x80) << 24)
					| ((wchar_t)(utf8[2] ^ 0x80) << 18)
					| ((wchar_t)(utf8[3] ^ 0x80) << 12)
					| ((wchar_t)(utf8[4] ^ 0x80) << 6)
					| (wchar_t)(utf8[5] ^ 0x80);
				return 6;
			}
			else {
				return INVALID_INPUT;
			}
		}

		int WcharToUtf8(wchar_t wch, unsigned char* utf8, int size) {
			int count = 0;
			if (wch < 0x80)
				count = 1;
			else if (wch < 0x800)
				count = 2;
			else if (wch < 0x10000)
				count = 3;
			else if (wch < 0x200000)
				count = 4;
			else if (wch < 0x4000000)
				count = 5;
			else if (wch <= 0x7fffffff)
				count = 6;
			else
				return INVALID_INPUT;

			if (size < count)
				return SMALL_OUTPUT;

			switch (count) {
			case 6: 
				utf8[5] = 0x80 | (wch & 0x3f); wch = wch >> 6; wch |= 0x4000000;
			case 5: 
				utf8[4] = 0x80 | (wch & 0x3f); wch = wch >> 6; wch |= 0x200000;
			case 4: 
				utf8[3] = 0x80 | (wch & 0x3f); wch = wch >> 6; wch |= 0x10000;
			case 3: 
				utf8[2] = 0x80 | (wch & 0x3f); wch = wch >> 6; wch |= 0x800;
			case 2: 
				utf8[1] = 0x80 | (wch & 0x3f); wch = wch >> 6; wch |= 0xc0;
			case 1: utf8[0] = (unsigned char)(wch&0xff);
			}
			return count;
		}
	}

	Locale::Locale(int category, const char* locale)
		: category_(category),
		locale_(locale?setlocale(category,locale):locale) {
	}

	Locale::~Locale() {
		if (!locale_)
			setlocale(category_, locale_);
	}

	Unicode::Unicode() {
	}

	Unicode::Unicode(const char* s, const char* locale/* = NULL*/) {
		From(s, locale);
	}

	Unicode::~Unicode() {
		ws_.clear();
	}

	Unicode& Unicode::FromUtf8(const char* s) {
		wchar_t wch = 0;
		int cnt = -1, i = 0, len = strlen(s);
		while ((cnt = WcharFromUtf8((unsigned char*)(s + i), len, wch)) > 0) {
			ws_.append(1, wch);
			len -= cnt;
			i += cnt;
		}
		if (cnt == INVALID_INPUT)
			ws_.clear();
		return *this;
	}

	Unicode& Unicode::FromAnsi(const char* s, const char* locale) {
		Locale loc(LC_CTYPE, locale);
		size_t retval = mbstowcs(NULL, s, 0);
		if (retval != (size_t)(-1)) {
			ws_.resize(retval + 1);
			mbstowcs(&ws_[0], s, ws_.size());
			ws_[retval] = L'\0';
		}
		return *this;
	}

	Unicode& Unicode::From(const char* s, const char* locale/* = NULL*/) {
		ws_.clear();
		String loc(locale);
		if (loc.Exist("utf8", true) || loc.Exist("utf-8", true))
			FromUtf8(s);
		else
			FromAnsi(s, locale);
		return *this;
	}


	const wchar_t* Unicode::str() const {
		return ws_.c_str();
	}

	Unicode::operator const wstring& () const{
		return ws_;
	}


	Ansi::Ansi(const char* locale/* = NULL*/)
		: locale_(locale) {
	}

	Ansi::Ansi(const wchar_t* ws, const char* locale/* = NULL*/)
		: locale_(locale) {
		FromUnicode(ws);
	}

	Ansi::~Ansi() {
		s_.clear();
	}

	Ansi& Ansi::FromUnicode(const wchar_t* ws) {
		s_.clear();
		Locale loc(LC_CTYPE, locale_);
		size_t retval = wcstombs(NULL, ws, 0);
		if (retval != (size_t)(-1)) {
			s_.resize(retval + 1);
			wcstombs(&s_[0], ws, s_.size());
			s_[retval] = '\0';
		}
		return *this;
	}

	Ansi& Ansi::FromUtf8(const char* s) {
		s_.clear();
		Unicode unicode(s, "utf8");
		FromUnicode(unicode.str());
		return *this;
	}

	const char* Ansi::str() const {
		return s_.c_str();
	}

	Ansi::operator const string& () const{
		return s_;
	}

	Utf8::Utf8() {
	}

	Utf8::Utf8(const wchar_t* ws) {
		FromUnicode(ws);
	}

	Utf8::Utf8(const char* ansi, const char* locale/* = NULL*/) {
	}

	Utf8::~Utf8() {
		utf8_.clear();
	}

	Utf8& Utf8::FromUnicode(const wchar_t* ws) {
		utf8_.clear();
		unsigned char chs[6] = { 0 };
		int cnt = 0, i = 0;
		while (*(ws + i) != '\0' && (cnt = WcharToUtf8(*(ws + i), chs, 6)) > 0) {
			utf8_.append((char*)(&chs[0]), cnt);
			i++;
		}
		return *this;
	}

	Utf8& Utf8::FromAnsi(const char* ansi, const char* locale) {
		Unicode unicode(ansi, locale);
		FromUnicode(unicode.str());
		return *this;
	}

	const char* Utf8::str() const {
		return utf8_.c_str();
	}

	Utf8::operator const string&() const {
		return utf8_;
	}
} // namespace hlp



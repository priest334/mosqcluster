#pragma once

#ifndef __BASE_STRINGS_STRING_CONVERT_H__
#define __BASE_STRINGS_STRING_CONVERT_H__

#include <string>

namespace hlp {
	using std::string;
	using std::wstring;

	class Locale {
	public:
		Locale(int category, const char* locale);
		~Locale();
	private:
		int category_;
		const char* locale_;
	};

	class Unicode {
	public:
		Unicode();
		Unicode(const char* s, const char* locale = NULL);
		~Unicode();

	//protected:
		Unicode& FromUtf8(const char* s);
		Unicode& FromAnsi(const char* s, const char* locale);

	public:
		Unicode& From(const char* s, const char* locale = NULL);

		const wchar_t* str() const;
		operator const wstring&() const;
	private:
		wstring ws_;
	};

	class Ansi {
	public:
		explicit Ansi(const char* locale = NULL);
		Ansi(const wchar_t* ws, const char* locale = NULL);
		~Ansi();

		Ansi& FromUnicode(const wchar_t* ws);
		Ansi& FromUtf8(const char* s);

		const char* str() const;
		operator const string&() const;
	private:
		string s_;
		const char* locale_;
	};

	class Utf8 {
	public:
		Utf8();
		explicit Utf8(const wchar_t* ws);
		explicit Utf8(const char* ansi, const char* locale/* = NULL*/);
		~Utf8();

		Utf8& FromUnicode(const wchar_t* ws);
		Utf8& FromAnsi(const char* ansi, const char* locale);

		const char* str() const;
		operator const string&() const;

	private:
		string utf8_;
	};
} // namespace hlp

#endif // __BASE_STRINGS_STRING_CONVERT_H__


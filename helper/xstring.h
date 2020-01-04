#pragma once

#include <stdlib.h>
#include <string.h>
#include <string>

namespace hlp {
	class xstring {
	public:
		typedef int size_type;
		xstring() : buffer_(NULL), size_(0), buffer_size_(0) {
		}

		~xstring() {
			free(buffer_);
		}

		xstring& resize_buffer(size_type size) {
			if (size <= 0)
				return *this;
			if (buffer_ == NULL) {
				buffer_ = (char*)malloc(size);
				buffer_size_ = size;
			} else if (size > buffer_size_) {
				buffer_ = (char*)realloc(buffer_, size);
				buffer_size_ = size;
			}
			return *this;
		}

		xstring& assign(const char* str, size_t size) {
			if (str == NULL || size <= 0) {
				size_ = 0;
				return *this;
			}
			resize_buffer(size + 1);
			memcpy(buffer_, str, size);
			buffer_[size] = '\0';
			size_ = size;
			return *this;
		}

		xstring& assign(const char* str) {
			return assign(str, strlen(str));
		}

		xstring& append(const char* str, size_type size) {
			if (str == NULL || size <= 0) {
				return *this;
			}
			resize_buffer(size_ + size + 1);
			memcpy(buffer_ + size_, str, size);
			buffer_[size_ + size] = '\0';
			size_ = size_ + size;
			return *this;
		}

		xstring& append(size_type size, char ch) {
			if (size == 0) {
				return *this;
			}
			resize_buffer(size_ + size + 1);
			memset(buffer_ + size_, ch, size);
			buffer_[size_ + size] = '\0';
			size_ = size_ + size;
			return *this;
		}

		xstring substr(size_type offset, size_type count = -1) const {
			xstring sub;
			if (offset >= size_ || offset < 0)
				return sub;
			if (count < 0 || count > (size_ - offset)) {
				return sub.assign(buffer_ + offset, size_ - offset);
			} else {
				return sub.assign(buffer_ + offset, count);
			}
		}

		xstring(const char* str) : buffer_(NULL), size_(0), buffer_size_(0) {
			assign(str, strlen(str));
		}

		xstring(const char* str, size_type length) : buffer_(NULL), size_(0), buffer_size_(0) {
			assign(str, length);
		}

		xstring(const xstring& str) : buffer_(NULL), size_(0), buffer_size_(0) {
			assign(str.buffer_, str.length());
		}

		xstring(const std::string& str) : buffer_(NULL), size_(0), buffer_size_(0) {
			assign(str.c_str(), (size_type)str.length());
		}

		xstring& operator=(const char* str) {
			assign(str, strlen(str));
			return *this;
		}

		xstring& operator=(const xstring& str) {
			assign(str.c_str(), str.length());
			return *this;
		}

		xstring& append(const char* str) {
			append(str, strlen(str));
			return *this;
		}

		xstring& append(const xstring& str) {
			append(str.c_str(), str.length());
			return *this;
		}

		xstring& append(const std::string& str) {
			append(str.c_str(), (size_type)str.length());
			return *this;
		}

		char operator[](int index) const {
			if (index < 0 || index >= size_)
				return '\0';
			return buffer_[index];
		}

		bool operator<(const xstring& str) const {
			for (size_type i = 0; i < size_ && i < str.size_; i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] < str[i];
			}
			return size_ < str.length();
		}

		bool operator<(const std::string& str) const {
			for (size_type i = 0; i < size_ && i < (size_type)str.length(); i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] < str[i];
			}
			return size_ < (size_type)str.length();
		}

		bool operator<(const char* str) const {
			size_type size = str ? strlen(str) : 0;
			for (size_type i = 0; i < size_ && i < size; i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] < str[i];
			}
			return size_ < size;
		}

		bool operator>(const xstring& str) const {
			for (size_type i = 0; i < size_ && i < str.size_; i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] > str[i];
			}
			return size_ > str.length();
		}

		bool operator>(const std::string& str) const {
			for (size_type i = 0; i < size_ && i < (size_type)str.length(); i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] > str[i];
			}
			return size_ > (size_type)str.length();
		}

		bool operator>(const char* str) const {
			size_type size = str ? strlen(str) : 0;
			for (size_type i = 0; i < size_ && i < size; i++) {
				if (buffer_[i] == str[i])
					continue;
				return buffer_[i] > str[i];
			}
			return size_ > size;
		}

		bool operator==(const xstring& str) const {
			if (size_ != str.size_)
				return false;
			for (size_type i = 0; i < size_; i++) {
				if (buffer_[i] != str[i])
					return false;
			}
			return true;
		}

		bool operator==(const std::string& str) const {
			if (size_ != (size_type)str.length())
				return false;
			for (size_type i = 0; i < size_; i++) {
				if (buffer_[i] != str[i])
					return false;
			}
			return true;
		}

		bool operator==(const char* str) const {
			size_type size = strlen(str);
			if (size_ != size)
				return false;
			for (size_type i = 0; i < size_; i++) {
				if (buffer_[i] != str[i])
					return false;
			}
			return true;
		}

		bool empty() const {
			return (size_ <= 0);
		}
		
		char const* c_str() const {
			const char* nil = "";
			if (buffer_ == NULL || size_ <= 0)
				return nil;
			return buffer_;
		}

		size_type length() const {
			return size_;
		}

	private:
		char* buffer_;
		size_type size_;
		size_type buffer_size_;
	};

	template <class OStreamType>
	OStreamType& operator<<(OStreamType& oss, const xstring& str) {
		oss << str.c_str();
		return oss;
	}
} // namespace hlp
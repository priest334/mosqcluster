#pragma once

#ifndef __LOCAL_STREM_H__
#define __LOCAL_STREM_H__

#include <memory.h>
#include <stdlib.h>
#include <vector>
#include <string>


class ILocalStream {
public:
	typedef unsigned char element_type;
	typedef unsigned int size_type;

	ILocalStream(const void* buffer, size_type size) : buffer_((element_type*)buffer), size_(size), offset_(0) {
		size_ = size;
	}
	
	virtual ~ILocalStream() {
	}

	template<class _Type>
	ILocalStream& operator>>(_Type& value) {
		size_type size = sizeof(_Type);
		element_type* cur = buffer_ + offset_;
		*cur = value;
		offset_ += size;
		return *this;
	}


	ILocalStream& operator>>(std::string& value) {
		size_type size = 0;
		(*this)>>(size);
		element_type* cur = buffer_ + offset_;
		value.assign((char*)cur, size);
		offset_ += size;
		return *this;
	}

private:
	element_type* buffer_;
	size_type size_;
	size_type offset_;
};


class OLocalStream {
public:
	typedef unsigned char element_type;
	typedef element_type* _etype_ptr;
	typedef unsigned int size_type;

	OLocalStream(size_type max_size) : size_(0) {
		buffer_ = (element_type*)malloc(max_size);
	}
	
	virtual ~OLocalStream() {
	}

	template<class _Type>
	OLocalStream& operator<<(const _Type& value) {
		size_type size = sizeof(_Type);
		element_type* cur = buffer_ + size_;
		*((_Type*)cur) = value;
		size_ += size;
		return *this;
	}

	OLocalStream& operator<<(const std::string& value) {
		size_type size = value.length();
		(*this)<<(size);
		element_type* p = (element_type*)value.c_str();
		element_type* cur = buffer_ + size_;
		for (size_type i = 0; i < size; i++) {
			*(cur + i) = *(p + i);
		}
		size_ += size;
		return *this;
	}


	element_type const* buffer() const {
		return buffer_;
	}

	element_type* buffer() {
		return buffer_;
	}

	size_type size() const {
		return size_;
	}

private:
	element_type* buffer_;
	size_type size_;
};

#endif // __LOCAL_STREM_H__


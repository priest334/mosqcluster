#include "native_file.h"

#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <streambuf>

using namespace std;

#ifndef unused_vairable
#define unused_vairable(x)
#endif

namespace {
	size_t Hash(const char* key, size_t size) {
		size_t hash = 0;
		for (size_t i = 0; i < size; i++) {
			hash = hash * 131 + *(key + i);
		}
		return (hash & 0xffff);
	}

	size_t Hash(const string& key) {
		return Hash(key.c_str(), key.length());
	}
}
namespace hlp {

	void NativeFile::Write(const NativeFile::Item& item) {
		fseek(fpr_, 0, SEEK_END);
		time_t now = time(0);
		size_t hash = Hash(item.key) * sizeof(size_t), addr = 0, end = (size_t)ftell(fpr_);
		fseek(fpr_, hash, SEEK_SET);
		fread(&addr, 1, 4, fpr_);
		if (addr != 0) {
			while (addr != 0) {
				fseek(fpr_, addr, SEEK_SET);
				NativeFile::Item xr;
				ReadItem(xr);
				if (xr.key == item.key) {
					if (xr.capcity < item.capcity) {
						addr = xr.next;
						xr.key = "";
						xr.value = "";
						xr.expires_in = 0;
						WriteItem(addr, xr);
						continue;
					}
					else {
						xr.expires_in = item.expires_in;
						xr.key = item.key;
						xr.value = item.value;
						WriteItem(addr, xr);
					}
					return;
				}
				if (xr.capcity >= item.capcity) {
					if (now > xr.expires_in) {
						xr.expires_in = item.expires_in;
						xr.key = item.key;
						xr.value = item.value;
						WriteItem(addr, xr);
						return;
					}
				}
				if (xr.next == 0) {
					fseek(fpw_, addr + sizeof(xr.capcity), SEEK_SET);
					fwrite(&end, 1, sizeof(end), fpw_);
					fflush(fpw_);
					WriteItem(end, item);
					return;
				}
				addr = xr.next;
			}
		}
		else {
			fseek(fpw_, hash, SEEK_SET);
			fwrite((char*)&end, 1, 4, fpw_);
			fflush(fpw_);
			WriteItem(end, item);
		}
	}

	bool NativeFile::Read(NativeFile::Item& item) {
		string content;
		size_t hash = Hash(item.key) * sizeof(size_t), addr = 0;
		fseek(fpr_, hash, SEEK_SET);
		fread(&addr, 1, sizeof(addr), fpr_);
		if (addr != 0) {
			while (addr != 0) {
				fseek(fpr_, addr, SEEK_SET);
				NativeFile::Item xr;
				ReadItem(xr);
				if (item.key == xr.key) {
					item.capcity = xr.capcity;
					item.expires_in = xr.expires_in;
					item.next = xr.next;
					item.value = xr.value;
					return true;
				}
				addr = xr.next;
			}
		}
		return false;
	}

	void NativeFile::WriteItem(size_t addr, const NativeFile::Item& item) {
		//os << xr.capcity << xr.next << item.expires_in << item.key.length() << item.key << item.value.length() << item.value;
		fseek(fpw_, addr, SEEK_SET);
		fwrite((char*)&item.capcity, 1, sizeof(item.capcity), fpw_);
		fwrite((char*)&item.next, 1, sizeof(item.next), fpw_);
		fwrite((char*)&item.expires_in, 1, sizeof(item.expires_in), fpw_);
		size_t len = item.key.length();
		fwrite((char*)&len, 1, sizeof(len), fpw_);
		fwrite(item.key.c_str(), 1, len, fpw_);
		len = item.value.length();
		fwrite((char*)&len, 1, sizeof(len), fpw_);
		fwrite(item.value.c_str(), 1, len, fpw_);
		fflush(fpw_);
	}

	void NativeFile::ReadItem(NativeFile::Item& item) {
		fread((char*)&item.capcity, 1, sizeof(item.capcity), fpr_);
		fread((char*)&item.next, 1, sizeof(item.next), fpr_);
		fread((char*)&item.expires_in, 1, sizeof(item.expires_in), fpr_);
		size_t len = 0;
		fread((char*)&len, 1, sizeof(len), fpr_);
		if (len > 0) {
			item.key.resize(len);
			fread(&item.key[0], 1, len, fpr_);
		}
		len = 0;
		fread((char*)&len, 1, sizeof(len), fpr_);
		if (len > 0) {
			item.value.resize(len);
			fread(&item.value[0], 1, len, fpr_);
		}
	}

	NativeFile::NativeFile() : fpr_(0), fpw_(0) {
	}

	NativeFile::~NativeFile() {
		if (fpr_) {
			fclose(fpr_);
		}
		if (fpw_) {
			fclose(fpw_);
		}
	}

	NativeFile::NativeFile(const string& file) : file_(file) {
		Open(file_);
	}

	void NativeFile::Open(const string& file) {
		file_ = file;
		struct stat buf;
		if (0 != stat(file_.c_str(), &buf) || buf.st_size == 0) {
			FILE* f = fopen(file_.c_str(), "wb");
			const int size = 0x10000 * sizeof(size_t);
			char* init = new char[size];
			memset(init, 0, size);
			fwrite(init, 1, size, f);
			delete[] init;
			fclose(f);
		}

		fpr_ = fopen(file_.c_str(), "rb");
		fpw_ = fopen(file_.c_str(), "rb+");
	}

	void NativeFile::Set(const string& key, const string& value, time_t expires_in) {
		Item item;
		item.capcity = sizeof(size_t) * 4 + sizeof(time_t) + key.length() + value.length();
		item.capcity = (item.capcity / 16 + 1) * 16;
		item.next = 0;
		item.expires_in = (expires_in >= 0) ? (time(NULL) + expires_in) : -1;
		item.key = key;
		item.value = value;
		Write(item);
	}

	string NativeFile::Get(const string& key, time_t& expires_in, bool cleanup) {
		unused_vairable(cleanup);
		time_t now = time(NULL);
		Item item;
		item.capcity = 0;
		item.next = 0;
		item.expires_in = 0;
		item.key = key;
		item.value = "";
		if (Read(item)) {
			if (item.expires_in >= 0 && now < item.expires_in) {
				expires_in = item.expires_in;
				return item.value;
			}
		}
		return string();
	}

} // namespace hlp



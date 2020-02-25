#ifndef __CRYPTO_BASE32_H__
#define __CRYPTO_BASE32_H__

#include <string>
#include "crypto/types.h"

#define BASE32_KEY_LENGTH 32
#define BASE32_INDEX_LENGTH 256

namespace hlp {
	namespace crypt {

		using std::string;

		class BASE32 {
			typedef char Base32Keys[BASE32_KEY_LENGTH];
			typedef uchar_t Base32Index[BASE32_INDEX_LENGTH];
		public:
			BASE32();
			int SetKeys(const string& keys);
			string Encrypt(const string& str);
			string Decrypt(const string& str);
		private:
			Base32Keys keys_;
			Base32Index index_;
		};
	} // namespace crypt
} // namespace hlp

#endif // __CRYPTO_BASE32_H__

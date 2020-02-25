#ifndef __CRYPTO_BASE64_H__
#define __CRYPTO_BASE64_H__

#include <string>
#include "crypto/types.h"
using std::string;


#define BASE64_KEY_LENGTH 64
#define BASE64_INDEX_LENGTH 256

namespace hlp {
	namespace crypt {

		class Base64 {
			typedef struct base64_keys_s {
				char keys_[BASE64_KEY_LENGTH];
				operator char*() {
					return &keys_[0];
				}
			} Base64Keys;

			typedef struct base64_index_s {
				uchar_t index_[BASE64_INDEX_LENGTH];
				operator uchar_t*() {
					return &index_[0];
				}
			} Base64Index;
		public:
			Base64();
			int SetKeys(const string& keys);
			string Encrypt(const string& str);
			string Decrypt(const string& str);
		private:
			Base64Keys keys_;
			Base64Index index_;
		};
	} // namespace crypt
} // namespace hlp


#endif // __CRYPTO_BASE64_H__

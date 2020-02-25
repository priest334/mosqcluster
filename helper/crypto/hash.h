
#ifndef __CRYPTO_HASH_H__
#define __CRYPTO_HASH_H__

#include <string>
#include "crypto/types.h"

namespace hlp {
	namespace crypt {
		using std::string;

		string Hash(const string& name, const string& src);

		string md5(const string& src);
		string sha1(const string& src);

	} // namespace crypt
}// namespace hlp


#endif // __CRYPTO_HASH_H__


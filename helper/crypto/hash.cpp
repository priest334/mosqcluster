#include "crypto/hash.h"

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include "crypto/hexstr.h"

namespace hlp {
	namespace crypt {
		string Hash(const string& name, const string& src) {
			EVP_MD_CTX mdctx;
			const EVP_MD* md;

			OpenSSL_add_all_digests();
			md = EVP_get_digestbyname(name.c_str());
			if (!md) {
				return src;
			}

			unsigned int outlen;
			StringBytes bytes;
			bytes.resize(EVP_MAX_MD_SIZE);

			EVP_MD_CTX_init(&mdctx);
			EVP_DigestInit_ex(&mdctx, md, NULL);
			EVP_DigestUpdate(&mdctx, src.c_str(), src.length());
			EVP_DigestFinal_ex(&mdctx, &bytes[0], &outlen);
			EVP_MD_CTX_cleanup(&mdctx);

			bytes.resize(outlen);
			return Bytes2HexStr(bytes);
		}

		string md5(const string& src) {
			StringBytes bytes(src.begin(), src.end()), out;
			out.resize(MD5_DIGEST_LENGTH);
			MD5(bytes.c_str(), src.length(), &out[0]);
			return Bytes2HexStr(out);
		}

		string sha1(const string& src) {
			StringBytes bytes(src.begin(), src.end()), out;
			out.resize(SHA_DIGEST_LENGTH);
			SHA1(bytes.c_str(), src.length(), &out[0]);
			return Bytes2HexStr(out);
		}

	} // namespace crypt
} // namespace hlp




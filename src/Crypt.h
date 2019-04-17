#ifndef SRC_CRYPT_H_
#define SRC_CRYPT_H_
#include <string>

#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/JSON/Object.h"

using namespace Poco::Crypto;
using std::string;

class Crypt {
 private:
    Cipher* cipher;
    RSADigestEngine* digestEngine;
    CipherFactory &factory = CipherFactory::defaultFactory();
    string JSONToString(Poco::JSON::Object::Ptr JSON);

 public:
    Crypt(string publicKey, string private_key);
    explicit Crypt(string publicKey);
    ~Crypt();

    string encrypt(string input);
    string decrypt(string input);
    string sign(Poco::JSON::Object::Ptr JSON);
    bool verify(Poco::JSON::Object::Ptr JSON, string digest);
};
#endif /* SRC_CRYPT_H_ */

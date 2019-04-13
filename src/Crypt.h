#ifndef SRC_CRYPT_H_
#define SRC_CRYPT_H_
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/JSON/Object.h"

using namespace Poco::Crypto;

class Crypt {
 private:
    Cipher* cipher;
    RSADigestEngine* digestEngine;
    CipherFactory &factory = CipherFactory::defaultFactory();

    std::string jsonToString(Poco::JSON::Object::Ptr json);

 public:
    Crypt(std::string publicKey, std::string privateKey);
    explicit Crypt(std::string publicKey);
    ~Crypt();

    std::string encrypt(std::string a);
    std::string decrypt(std::string a);
    std::string sign(Poco::JSON::Object::Ptr json);
    bool verify(Poco::JSON::Object::Ptr json, std::string digest);
};

#endif /* SRC_CRYPT_H_ */

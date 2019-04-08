#include "Poco/Crypto/CryptoStream.h"
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "Poco/Crypto/DigestEngine.h"
#include <iostream>
#include <string.h>
#include <stdexcept>
#include <Poco/JSON/Object.h>
#include "crypt.h"

/*  Usage
 *      Create an instance of the crypt class.
 *      Then use the methods given to encrypt
 *      and decrypt std::string
 *
 *      Check main functions for better usage
 * */

using namespace Poco::Crypto;

class Crypt{

private:
    Cipher* cipher;
    RSADigestEngine* digestEngine;
    CipherFactory &factory = CipherFactory::defaultFactory();

    std::string jsonToString(Poco::JSON::Object::Ptr json) {
        std::stringstream data;
        json->stringify(data);
        return data.str();
    }

public:
    //Create an instance and a cipher in one
    Crypt() {
        RSAKey key(RSAKey(RSAKey::KL_2048, RSAKey::EXP_LARGE));
        digestEngine = new RSADigestEngine(key);
        cipher = factory.createCipher(key);
    }

    Crypt(std::string publicKey, std::string privateKey) {
        RSAKey key(RSAKey(publicKey, privateKey));
        digestEngine = new RSADigestEngine(key);
        cipher = factory.createCipher(key);
    }

    // Creates an instance for when only the public key is known
    Crypt(std::string publicKey) {
        RSAKey key(publicKey);
        digestEngine = new RSADigestEngine(key);
        cipher = factory.createCipher(key);

    }

    //Encryptes a string
    std::string encrypt(std::string a){
        return cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
    }
    //Decrypt a string
    std::string decrypt(std::string a){
        return cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);;
    }

    // Signs a string with private key. Returns a hex string
    std::string sign(Poco::JSON::Object::Ptr json) {
        digestEngine->reset();
        digestEngine->update(jsonToString(json));
        std::vector<unsigned char> v = digestEngine->signature();
        return digestEngine->digestToHex(v);
    }

    // Verify digest against data. Input must be a hex string
    bool verify(Poco::JSON::Object::Ptr json, std::string digest) {
        digestEngine->reset();
        digestEngine->update(jsonToString(json));
        std::vector<unsigned char> v = digestEngine->digestFromHex(digest);
        return digestEngine->verify(v);
    }

};



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
    
    // Signs a string with private key
    std::string sign(std::string a) {
        digestEngine->reset();
        digestEngine->update(a);
        std::vector<unsigned char> v = digestEngine->signature();
        return std::string(v.begin(), v.end());
    }
    
    // Verify digest against data
    bool verify(std::string data, std::string digest) {
        digestEngine->reset();
        digestEngine->update(data);
        std::vector<unsigned char> v(digest.begin(), digest.end());
        return digestEngine->verify(v);
    }
    
};



#include "Poco/Crypto/CryptoStream.h"
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/RSAKey.h"
#include <iostream>
#include <string.h>
#include <stdexcept>
#include "crypt.h"

/*  Usage
 *      Create an instance of the crypt class.
 *      Then use the methods given to encrypt
 *      and decrypt std::strings
 *
 *      Check main functions for better usage
 * */

using namespace Poco::Crypto;

class Crypt{

private:
    Cipher* cipher;
    CipherFactory &factory = CipherFactory::defaultFactory();

public:
    //Create an instance and a cipher in one
    Crypt() {
        RSAKey key(RSAKey(RSAKey::KL_2048, RSAKey::EXP_LARGE));
        cipher = factory.createCipher(key);
    }

    Crypt(std::string publicKey, std::string privateKey) {
        RSAKey key(RSAKey(publicKey, privateKey));
        cipher = factory.createCipher(key);
    }

    //Encryptes a string
    std::string encrypt(std::string a){
        return cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);;
    }
    //Decrypt a string
    std::string decrypt(std::string a){
        return cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);;
    }

};
#include <string>
#include "./Crypt.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Poco/Crypto/CryptoStream.h"
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/JSON/Object.h"

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
    /*! \brief Creates a Crypt class with both public and private key
     *
     * Used by an agent in order to decrypt and sign messages.
     *
     * \return Crypt privateCrypt
     */
    Crypt(std::string publicKey, std::string privateKey) {
        RSAKey key(RSAKey(publicKey, privateKey));
        digestEngine = new RSADigestEngine(key);
        cipher = factory.createCipher(key);
    }

    /*! \brief Creates a Crypt when only the public key is known
     *
     * Used by an agent in order to encrypt messages to recipients.
     *
     * \return Crypt publicCrypt
     */
    explicit Crypt(std::string publicKey) {
        RSAKey key(publicKey);
        digestEngine = new RSADigestEngine(key);
        cipher = factory.createCipher(key);
    }

    /*! \brief Encrypts a string
     *
     * The encryption can only be done with the intended recipients PUBLIC key.
     *
     * \return std::string encrypted
     */
    std::string encrypt(std::string a) {
        return cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
    }

    /*! \brief Decrypts a string
     *
     * The decryption can only be done with the intended recipients PRIVATE key.
     *
     * \return std::string decrypted
     */
    std::string decrypt(std::string a) {
        return cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);;
    }

    /*! \brief Sign the JSON object and returns the signature
     *
     * Note that the signature created gets converted to a hex string before
     * it is returned!
     *
     * \return std::string signature
     */
    std::string sign(Poco::JSON::Object::Ptr json) {
        digestEngine->reset();
        digestEngine->update(jsonToString(json));
        std::vector<unsigned char> v = digestEngine->signature();
        return digestEngine->digestToHex(v);
    }

    /*! \brief Verifies the JSON against the signature string
     *
     * Note that the input is and must be a hex string, which gets converted to a
     * unsigned char vector before being verified against the JSON.
     *
     * \return bool verified
     */
    bool verify(Poco::JSON::Object::Ptr json, std::string digest) {
        digestEngine->reset();
        digestEngine->update(jsonToString(json));
        std::vector<unsigned char> v = digestEngine->digestFromHex(digest);
        return digestEngine->verify(v);
    }
};

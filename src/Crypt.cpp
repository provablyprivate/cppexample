#include "./Crypt.h"

/*  Usage
 *      Create an instance of the crypt class.
 *      Then use the methods given to encrypt
 *      and decrypt std::string
 *
 *      Check main functions for better usage
 * */


std::string Crypt::jsonToString(Poco::JSON::Object::Ptr json) {
    std::stringstream data;
    json->stringify(data);
    return data.str();
}

/*! \brief Creates a Crypt class with both public and private key
 *
 * Used by an agent in order to decrypt and sign messages.
 *
 * \return Crypt privateCrypt
 */
Crypt::Crypt(std::string publicKey, std::string privateKey) {
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
Crypt::Crypt(std::string publicKey) {
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
std::string Crypt::encrypt(std::string a) {
    return cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
}

/*! \brief Decrypts a string
 *
 * The decryption can only be done with the intended recipients PRIVATE key.
 *
 * \return std::string decrypted
 */
std::string Crypt::decrypt(std::string a) {
    return cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);;
}

/*! \brief Sign the JSON object and returns the signature
 *
 * Note that the signature created gets converted to a hex string before
 * it is returned!
 *
 * \return std::string signature
 */
std::string Crypt::sign(Poco::JSON::Object::Ptr json) {
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
bool Crypt::verify(Poco::JSON::Object::Ptr json, std::string digest) {
    digestEngine->reset();
    digestEngine->update(jsonToString(json));
    std::vector<unsigned char> v = digestEngine->digestFromHex(digest);
    return digestEngine->verify(v);
}

#include "./Crypt.h"

#include <string>
#include <vector>

/*  Usage
 *      Create an instance of the crypt class.
 *      Then use the methods given to encrypt
 *      and decrypt string
 *
 *      Check main functions for better usage
 * */

/*! \brief Returns the JSON as a string
 *
 * \return string data
 */
string Crypt::JSONToString(Poco::JSON::Object::Ptr JSON) {
    std::stringstream data;
    JSON->stringify(data);

    return data.str();
}

/*! \brief Creates a Crypt class with both public and private key
 *
 * Used by an agent in order to decrypt and sign messages.
 *
 * \return Crypt privateCrypt
 */
Crypt::Crypt(string public_key, string private_key) {
    RSAKey key(RSAKey(public_key, private_key));
    digestEngine = new RSADigestEngine(key);
    cipher = factory.createCipher(key);
}

/*! \brief Creates a Crypt when only the public key is known
 *
 * Used by an agent in order to encrypt messages to recipients.
 *
 * \return Crypt publicCrypt
 */
Crypt::Crypt(string public_key) {
    RSAKey key(public_key);
    digestEngine = new RSADigestEngine(key);
    cipher = factory.createCipher(key);
}

/*! \brief Encrypts a string
 *
 * The encryption can only be done with the intended recipients PUBLIC key.
 *
 * \return string encrypted
 */
string Crypt::encrypt(string input) {
    return cipher->encryptString(input, Cipher::ENC_BASE64_NO_LF);
}

/*! \brief Decrypts a string
 *
 * The decryption can only be done with the intended recipients PRIVATE key.
 *
 * \return string decrypted
 */
string Crypt::decrypt(string input) {
    return cipher->decryptString(input, Cipher::ENC_BASE64_NO_LF);;
}

/*! \brief Sign the JSON object and returns the signature
 *
 * Note that the signature created gets converted to a hex string before
 * it is returned!
 *
 * \return string signature
 */
string Crypt::sign(Poco::JSON::Object::Ptr JSON) {
    digestEngine->reset();
    digestEngine->update(JSONToString(JSON));
    std::vector<unsigned char> vector = digestEngine->signature();

    return digestEngine->digestToHex(vector);
}

/*! \brief Verifies the JSON against the signature string
 *
 * Note that the input is and must be a hex string, which gets converted to a
 * unsigned char vector before being verified against the JSON.
 *
 * \return bool verified
 */
bool Crypt::verify(Poco::JSON::Object::Ptr JSON, string digest) {
    digestEngine->reset();
    digestEngine->update(JSONToString(JSON));
    std::vector<unsigned char> vector = digestEngine->digestFromHex(digest);

    return digestEngine->verify(vector);
}

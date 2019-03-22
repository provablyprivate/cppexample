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

class Crypt{
    Poco::Crypto::Cipher* cipher;
    public: 
    //Create an instance and a cipher in one
    Crypt(std::string password){
        std::cout << "Created a new instance of crypt using a password" << "\n";
        this->createCipher(password);
    }
    Crypt(){
        std::cout << "Created a new instance of the crypt class" << "\n";
    }
    
    //Create a cipher that is used to decrypt and encrypt messages
    Poco::Crypto::Cipher* createCipher(std::string password, std::string salt, std::string digest, std::string method){
        Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
        Poco::Crypto::CipherKey key(method, password, salt, ITERATIONS, digest);
        cipher = factory.createCipher(key);
        return cipher;
    }

    //Create a cipher that is used to decrypt and encrypt messages with default settings
    Poco::Crypto::Cipher* createCipher(std::string password){
        Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
        std::string salt(SALT);
        std::string digest (DIGEST);
        Poco::Crypto::CipherKey key(METHOD, password, salt, ITERATIONS, digest);
        cipher = factory.createCipher(key);
        return cipher;
    }

    //Encryptes a string
    std::string encrypt(std::string a, Poco::Crypto::Cipher* cipher){
        std::string encrypted = cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
        return encrypted;
    }
    
    //Encryptes a string
    std::string encrypt(std::string a){
        std::string encrypted = cipher->encryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
        return encrypted;
    }

    //Decrypt a string
    std::string decrypt(std::string a, Poco::Crypto::Cipher* cipher){
        std::string decrypt = cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
        return decrypt;
    }
    //Decrypt a string
    std::string decrypt(std::string a){
        std::string decrypt = cipher->decryptString(a, Poco::Crypto::Cipher::ENC_BASE64);
        return decrypt;
    }

};

int main(){
    std::string message = "My secret message";
    Crypt* childCrypt = new Crypt("password");
    std::string encryptedMessage = childCrypt->encrypt(message);
    std::cout << encryptedMessage << "\n" ;
    std::string decryptedMessage = childCrypt->decrypt(encryptedMessage);
    std::cout << decryptedMessage << "\n" ;
    return 0;
}


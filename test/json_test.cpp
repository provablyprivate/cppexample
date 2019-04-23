#include <iostream>
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "../src/crypt.cpp"




using namespace Poco;

int main() {
    Poco::FileInputStream fileStream("./test/pdata.json", std::ios::binary);

    std::string json_data;
    StreamCopier::copyToString(fileStream, json_data);

    JSON::Parser parser;
    Dynamic::Var result = parser.parse(json_data);
    JSON::Object::Ptr object = result.extract<JSON::Object::Ptr>();
    std::string encryptedValue = object.get()->get("value");

    std::cout << encryptedValue << std::endl;

    std::string message = "some private data";
    auto * cryptPubPr = new Crypt("./test/rsa_key.pub","./test/rsa_key");

//    std::string encryptedMessage = crypt->encrypt(message);
//    std::cout << encryptedMessage << "\n" ;
    std::string decryptedMessage = cryptPubPr->decrypt(encryptedValue);
    std::cout << decryptedMessage << "\n";
    
    
    // Encrypt when only the public key is known:    
    auto * cryptPub = new Crypt("./test/rsa_key.pub");
    std::string encryptedMessage = cryptPub->encrypt(message);
    
    // Decrypt it with private key:
    std::string decryptedMessage2 = cryptPubPr->decrypt(encryptedMessage);
    std::cout << decryptedMessage2;
    
    
    
    // Sign (with private key):
    std::string data = "some data to sign";
    std::string dig = cryptPubPr->sign(data);
    
    // Verify digest against data (with only public key):
    bool success = cryptPub->verify(data, dig);
    if (success) std::cout << "OK";
    
    
   
    
    
    
    
}

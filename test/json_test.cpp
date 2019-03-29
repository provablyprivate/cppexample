#include <iostream>
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "../src/crypt.cpp"

using namespace Poco;

int main() {
    Poco::FileInputStream fileStream("../test/pdata.json", std::ios::binary);

    std::string json_data;
    StreamCopier::copyToString(fileStream, json_data);

    JSON::Parser parser;
    Dynamic::Var result = parser.parse(json_data);
    JSON::Object::Ptr object = result.extract<JSON::Object::Ptr>();
    std::string encryptedValue = object.get()->get("value");

    std::cout << encryptedValue << std::endl;

    std::string message = "some private data";
    auto * crypt = new Crypt("../test/rsa_key.pub","../test/rsa_key");

//    std::string encryptedMessage = crypt->encrypt(message);
//    std::cout << encryptedMessage << "\n" ;
    std::string decryptedMessage = crypt->decrypt(encryptedValue);
    std::cout << decryptedMessage << "\n" ;
}
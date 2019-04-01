#include <iostream>
#include <fstream>
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/RSADigestEngine.h"
#include "../../src/crypt.cpp"
#include <unistd.h>

using namespace Poco;
using namespace std;


// Creates generic JSON object
Dynamic::Var createVarJSON(string stringJSON) {
    JSON::Parser p;
    return p.parse(stringJSON);
}

// Get data from json
string getDataJSON(Dynamic::Var variable, string data) {
    JSON::Object::Ptr obj = variable.extract<JSON::Object::Ptr>();
    return obj->get(data).toString();
}

// Put data in json
void putDataJSON(Dynamic::Var variable, string key, string value) {
    JSON::Object::Ptr obj = variable.extract<JSON::Object::Ptr>();
    obj->set(key, value);
}

// Prints file contents
string read(string location) {
    string tmp = "";
    ifstream file(location);
    StreamCopier::copyToString(file, tmp);
    file.close();
    return tmp;
}

int main() {
    unsigned int time = 100000;  // sleep timer for "animation"
    Dynamic::Var jsonVar = createVarJSON("{}");  // Empty json to start with
    string tmp = ""; // temp string

    // Instantiates encryption classes for every type
    std::cout << "\n\nCreating crypt classes for all the types" << std::endl;
    auto * websiteCrypt = new Crypt("./test/communication/rsa-keys/website.pub","./test/communication/rsa-keys/website");
    // auto * parentCrypt = new Crypt("./test/communication/rsa-keys/parent.pub","./test/communication/rsa-keys/parent");
    // auto * childCrypt = new Crypt("./test/communication/rsa-keys/child.pub","./test/communication/rsa-keys/child");
    std::cout << "Classes created" << std::endl;
    usleep(time);

    std::cout << "\nThis is the websites policy:" << std::endl;
    std::cout << read("./test/communication/policy.data") << std::endl;
    usleep(time);

    std::cout << "Website now creates a JSON object with key POLICY and the above policy as value" << std::endl;
    putDataJSON(jsonVar, "POLICY", read("./test/communication/policy.data"));
    std::cout << "Done. This is the result:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;
    usleep(time);

    std::cout << "\n Next step is for the website to sign the json object." << std::endl;
    std::cout << "Done. heres the result: " << std::endl;
    string s = websiteCrypt->sign(jsonVar.toString());
    std::cout << hex_to_string(s) << std::endl;


     return 0;


//         Poco::FileInputStream fileStream("./test/pdata.json", std::ios::binary);
//
//     std::string json_data;
//     StreamCopier::copyToString(fileStream, json_data);
//
//     JSON::Parser parser;
//     Dynamic::Var result = parser.parse(json_data);
//     JSON::Object::Ptr object = result.extract<JSON::Object::Ptr>();
//     std::string encryptedValue = object.get()->get("value");
//
//     std::cout << encryptedValue << std::endl;
//
//     std::string message = "some private data";
//     auto * cryptPubPr = new Crypt("./test/rsa_key.pub","./test/rsa_key");
//
// //    std::string encryptedMessage = crypt->encrypt(message);
// //    std::cout << encryptedMessage << "\n" ;
//     std::string decryptedMessage = cryptPubPr->decrypt(encryptedValue);
//     std::cout << decryptedMessage << "\n";
//
//
//     // Encrypt when only the public key is known:
//     auto * cryptPub = new Crypt("./test/rsa_key.pub");
//     std::string encryptedMessage = cryptPub->encrypt(message);
//
//     // Decrypt it with private key:
//     std::string decryptedMessage2 = cryptPubPr->decrypt(encryptedMessage);
//     std::cout << decryptedMessage2;
//
//
//
//     // Sign (with private key):
//     std::string data = "some data to sign";
//     std::string dig = cryptPubPr->sign(data);
//
//     // Verify digest against data (with only public key):
//     bool success = cryptPub->verify(data, dig);
//     if (success) std::cout << "OK";
}

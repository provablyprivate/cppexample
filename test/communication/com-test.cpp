/*
 * !!!PLEASE READ!!!
 *
 * WHY THIS IS BAD:
 * The created JSON handlers are NOT used currently
 * Instead get and put functions are created below
 *
 * This needs to change
 *
 */

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
    unsigned int time = 1000000;                 // sleep timeri (ms) for "animation", change as needed
    Dynamic::Var jsonVar = createVarJSON("{}");  // Empty json to start with
    for (int i = 0; i < 80; ++i) {
        printf("\n");                            // Making space
    }

    // Instantiates encryption classes for every type
    std::cout << "\n\nCreating crypt classes for all the types\n" << std::endl;
    auto * webCrypt = new Crypt("./test/communication/rsa-keys/website.pub","./test/communication/rsa-keys/website");
    std::cout << "webCrypt    created" << std::endl;
    auto * webPubCrypt = new Crypt("./test/communication/rsa-keys/website.pub");
    std::cout << "webPubCrypt created" << std::endl;
    auto * parCrypt = new Crypt("./test/communication/rsa-keys/parent.pub","./test/communication/rsa-keys/parent");
    std::cout << "parCrypt    created" << std::endl;
    auto * parPubCrypt = new Crypt("./test/communication/rsa-keys/parent.pub");
    std::cout << "parPubCrypt created" << std::endl;
    auto * chiCrypt = new Crypt("./test/communication/rsa-keys/child.pub","./test/communication/rsa-keys/child");
    std::cout << "chiCrypt    created" << std::endl;
    auto * chiPubCrypt = new Crypt("./test/communication/rsa-keys/child.pub");
    std::cout << "chiPubCrypt created" << std::endl;
    std::cout << "\nAll classes created" << std::endl;
    usleep(time);


    /* T0 */
    std::cout << "\n\n########## T0: AT WEBSITE ##########" << std::endl;
    std::cout << "\nThis is the websites policy:" << std::endl;
    std::cout << read("./test/communication/policy.data") << std::endl;

    std::cout << "Website now creates a JSON object with key POLICY and the above policy as value" << std::endl;
    putDataJSON(jsonVar, "Type", "Policy");
    putDataJSON(jsonVar, "Value", read("./test/communication/policy.data"));
    std::cout << "Done. This is the result:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "\nNext step is for the website to sign the json object." << std::endl;
    string websiteSigned = webCrypt->sign(jsonVar.toString());
    std::cout << "Signed with webCrypt! The object (and sign) is sent to the parent" << std::endl;
    usleep(time);


    /* T1 */
    std::cout << "\n\n\n\n########### T1: AT PARENT ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "\nIt verifies the signature with the websites public key (webPubCrypt)"  << std::endl;
    webPubCrypt->verify(jsonVar.toString(), websiteSigned) ? std::cout << "OK!" : std::cout << "ERROR";

    std::cout << "\n\nWe assume the parent agrees to the policy. Thus adds its answer like this:" << std::endl;
    putDataJSON(jsonVar, "Type", "Consent");
    putDataJSON(jsonVar, "Value", "OK");
    putDataJSON(jsonVar, "Child", "1");
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "Next the parent signs the JSON and send it back to the website" << std::endl;
    string parentSigned = parCrypt->sign(jsonVar.toString());
    std::cout << "signed with parCrypt!" << std::endl;
    usleep(time);



    /* T2 */
    std::cout << "\n\n\n\n########### T2: AT WEBSITE ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "The website verifies the json with parents signature (parPubCrypt):" << std::endl;
    parPubCrypt->verify(jsonVar.toString(), parentSigned) ? std::cout << "OK!" : std::cout << "ERROR";
    std::cout << "\nIt then checks the target child and forwards the JSON (with par sign)" << std::endl;
    usleep(time);


    /* T3 */
    std::cout << "\n\n\n\n########## T3: AT CHILD ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "\nThis is the childs data:" << std::endl;
    std::cout << read("./test/communication/child.data") << std::endl;

    std::cout << "The child verifies the json with parents signature (parPubCrypt):" << std::endl;
    parPubCrypt->verify(jsonVar.toString(), parentSigned) ? std::cout << "OK!" : std::cout << "ERROR";
    std::cout << "\n\nIt then reads the JSON to find that the consent is OK" << std::endl;

    std::cout << "The child encrypts(sign??) its data with the websited public key (webPubCrypt)" << std::endl;
    string encToWeb = webPubCrypt->encrypt(read("./test/communication/child.data"));
    std::cout << "\nIt updates the json to look like this:" << std::endl;
    putDataJSON(jsonVar, "Type", "PDATA");
    putDataJSON(jsonVar, "Value", encToWeb);
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "Next the child signs the JSON and send it back to the website" << std::endl;
    string childSigned = chiCrypt->sign(jsonVar.toString());
    std::cout << "signed with chiCrypt!" << std::endl;
    usleep(time);


    /* T4 */
    std::cout << "\n\n\n\n########## T4: AT WEBSITE ##########" << std::endl;
    std::cout << "This JSON is recieved:" << std::endl;
    std::cout << jsonVar.toString() << std::endl;

    std::cout << "\nThe website verifies the json with childs signature (chiPubCrypt):" << std::endl;
    chiPubCrypt->verify(jsonVar.toString(), childSigned) ? std::cout << "OK!" : std::cout << "ERROR";
    std::cout << "\n\nIt then reads the JSON and decrypts the data (webCrypt):" << std::endl;
    string encData = getDataJSON(jsonVar, "Value");
    string decData = webCrypt->decrypt(encData);
    std::cout << decData << std::endl;

    std::cout << "\n\n\ndone!!!!" << std::endl;
    return 0;
}

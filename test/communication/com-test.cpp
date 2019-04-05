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
#include "../../src/jsonhandler.cpp"
#include <unistd.h>


using namespace Poco;
using namespace std;

// Prints file contents
string read(string location) {
    string tmp = "";
    ifstream file(location);
    StreamCopier::copyToString(file, tmp);
    file.close();
    return tmp;
}

// Verifies signatures
void verifySignature(Crypt* key, string signature, JSONHandler* JSON)
{
    key->verify(JSON->toString(), signature) ? std::cout << "OK!" : std::cout << "ERROR";
}

int main() {
    unsigned int time = 1000;                 // sleep timeri (ms) for "animation", change as needed
    for (int i = 0; i < 80; ++i) {
        printf("\n");                         // Making space
    }

    // Instantiates encryption classes for every type
    std::cout << "\n\nCreating crypt classes for all the types\n" << std::endl;
    Crypt * webCrypt = new Crypt("./test/communication/rsa-keys/website.pub", "./test/communication/rsa-keys/website");
    std::cout << "webCrypt    created" << std::endl;
    auto * webPubCrypt = new Crypt("./test/communication/rsa-keys/website.pub");
    std::cout << "webPubCrypt created" << std::endl;
    auto * parCrypt = new Crypt("./test/communication/rsa-keys/parent.pub", "./test/communication/rsa-keys/parent");
    std::cout << "parCrypt    created" << std::endl;
    auto * parPubCrypt = new Crypt("./test/communication/rsa-keys/parent.pub");
    std::cout << "parPubCrypt created" << std::endl;
    auto * chiCrypt = new Crypt("./test/communication/rsa-keys/child.pub", "./test/communication/rsa-keys/child");
    std::cout << "chiCrypt    created" << std::endl;
    auto * chiPubCrypt = new Crypt("./test/communication/rsa-keys/child.pub");
    std::cout << "chiPubCrypt created" << std::endl;
    std::cout << "\nAll classes created" << std::endl;
    usleep(time);


    /* T0 */
    std::cout << "\n\n\n\n########## T0: AT WEBSITE ##########" << std::endl;
    std::cout << "This is the websites policy:" << std::endl;
    std::cout << read("./test/communication/policy.data") << std::endl;

    std::cout << "Website now creates a JSON object with type Policy and the above policy as value" << std::endl;
    JSONHandler * jsonWeb = new JSONHandler();
    //Dynamic::Var jsonWeb = createVarJSON("{}");
    jsonWeb->put("Type", "Policy");
    jsonWeb->put( "Value", read("./test/communication/policy.data"));
    std::cout << "Done. This is the result:" << std::endl;
    std::cout << jsonWeb->toString() << std::endl;

    std::cout << "\nNext step is for the website to sign the json object." << std::endl;
    string webSign = webCrypt->sign(jsonWeb->toString());
    std::cout << "Signed with webCrypt! The JSON and sign is sent to the parent" << std::endl;
    usleep(time);


    /* T1 */
    std::cout << "\n\n\n\n########### T1: AT PARENT ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonWeb->toString() << std::endl;

    std::cout << "\nIt verifies the signature with the websites public key (webPubCrypt)"  << std::endl;
    verifySignature(webPubCrypt, webSign, jsonWeb);

    std::cout << "\n\nWe assume the parent agrees to the policy.\
        \nThus creating the following JSON:" << std::endl;
    JSONHandler * jsonPar = new JSONHandler();
    string consent = chiPubCrypt->encrypt("Permission given"); // Consent is encrypted
    jsonPar->put("Type", "Consent");
    jsonPar->put("Value", consent);
    jsonPar->put("Child", "1");
    jsonPar->put("Previous Signature", webSign);
    jsonPar->put("Previous JSON", jsonWeb->toString());
    std::cout << jsonPar->toString() << std::endl;

    std::cout << "Next the parent signs the JSON and send it back to the website" << std::endl;
    string parSign = parCrypt->sign(jsonPar->toString());
    std::cout << "signed with parCrypt!" << std::endl;
    usleep(time);



    /* T2 */
    std::cout << "\n\n\n\n########### T2: AT WEBSITE ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonPar->toString() << std::endl;

    std::cout << "The website verifies the json with parents signature (parPubCrypt):" << std::endl;
    verifySignature(parPubCrypt, parSign, jsonPar);
    std::cout << "\nIt then checks the target child and forwards the JSON (with par sign)" << std::endl;
    usleep(time);


    /* T3 */
    std::cout << "\n\n\n\n########## T3: AT CHILD ##########" << std::endl;
    std::cout << "The json recieved:" << std::endl;
    std::cout << jsonPar->toString() << std::endl;

    std::cout << "\nThis is the childs data:" << std::endl;
    std::cout << read("./test/communication/child.data") << std::endl;

    std::cout << "The child verifies the json with parents signature (parPubCrypt):" << std::endl;
    verifySignature(parPubCrypt, parSign, jsonPar);


    std::cout << "\n\nIt then decrypts its message from the JSON to find that the consent is OK:" << std::endl;
    std::cout << chiCrypt->decrypt(jsonPar->get("Value")) <<  std::endl;


    std::cout << "\n\nThe child creates a new JSON and does its things, encrypting pdata, signing, etc" << std::endl;
    std::cout << "The result looks like this like this:" << std::endl;
    JSONHandler * jsonChi = new JSONHandler();
    string encToWeb = webPubCrypt->encrypt(read("./test/communication/child.data"));
    jsonChi->put("Type", "Pdata");
    jsonChi->put("Value", encToWeb);
    jsonChi->put("Previous Signature", parSign);
    jsonChi->put("Previous JSON", jsonPar->toString());
    std::cout << jsonChi->toString() << std::endl;

    std::cout << "Next the child signs the JSON and send it back to the website" << std::endl;
    string chiSign = chiCrypt->sign(jsonChi->toString());
    std::cout << "signed with chiCrypt!" << std::endl;
    usleep(time);


    /* T4 */
    std::cout << "\n\n\n\n########## T4: AT WEBSITE ##########" << std::endl;
    std::cout << "This JSON is recieved:" << std::endl;
    std::cout << jsonChi->toString() << std::endl;

    std::cout << "\nThe website verifies the json with childs signature (chiPubCrypt):" << std::endl;
    verifySignature(chiPubCrypt, chiSign, jsonChi);
    std::cout << "\n\nIt then reads the JSON and decrypts the data (webCrypt):" << std::endl;
    string encData = jsonChi->get("Value");
    string decData = webCrypt->decrypt(encData);
    std::cout << decData << std::endl;

    std::cout << "\n\n\nDone!! For funzies, let us act as a 3rd party and recursively verify all messages sent, from the last JSON created at Child (see code for details):" << std::endl;

    printf("%s", "Verifying Child created JSON: ");
    verifySignature(chiPubCrypt, chiSign, jsonChi);

    printf("\n%s", "Verifying Parent created JSON: ");
    string prevSign = jsonChi->get("Previous Signature");
    Var prevJson = jsonChi->get("Previous JSON");
    parPubCrypt->verify(prevJson, prevSign) ? std::cout << "OK!\n" : std::cout << "ERROR";
    //
   // WHY ABOVE NOT WORKING
    std::cout << typeid(jsonChi->toString()).name() << std::endl;
    std::cout << typeid("A").name() << std::endl;
    std::cout << typeid("AB AB AB BA").name() << std::endl;
    std::cout << typeid(prevJson).name() << std::endl;
    std::cout << prevJson.toString() << std::endl;
    JSONHandler * p = new JSONHandler(prevJson.toString());
    // printf("\n%s", "Verifying Website created JSON: ");
    //prevJSON = getDataJSON(prevJSON, "Previous JSON");
    //string oldWebSign = getDataJSON(oldParJSON, "Previous Signature");
    //verifySignature(webPubCrypt, oldWebSign, oldWebJSON);


    return 0;
}

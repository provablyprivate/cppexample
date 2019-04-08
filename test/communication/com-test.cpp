#include <iostream>
#include <fstream>
#include "Poco/StreamCopier.h"
#include "../../src/crypt.cpp"
#include "../../src/jsonhandler.cpp"


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
void verifySignature(Crypt* key, string signature, JSON::Object::Ptr json)
{
    key->verify(json, signature) ? cout << "OK!" : cout << "ERROR";
}

int main() {
    for (int i = 0; i < 80; ++i) {
        printf("\n");                         // Making space
    }

    // Instantiates encryption classes for every type
    cout << "\n\nCreating crypt classes for all the types\n" << endl;
    Crypt * webCrypt = new Crypt("./test/communication/rsa-keys/website.pub", "./test/communication/rsa-keys/website");
    cout << "webCrypt    created" << endl;
    auto * webPubCrypt = new Crypt("./test/communication/rsa-keys/website.pub");
    cout << "webPubCrypt created" << endl;
    auto * parCrypt = new Crypt("./test/communication/rsa-keys/parent.pub", "./test/communication/rsa-keys/parent");
    cout << "parCrypt    created" << endl;
    auto * parPubCrypt = new Crypt("./test/communication/rsa-keys/parent.pub");
    cout << "parPubCrypt created" << endl;
    auto * chiCrypt = new Crypt("./test/communication/rsa-keys/child.pub", "./test/communication/rsa-keys/child");
    cout << "chiCrypt    created" << endl;
    auto * chiPubCrypt = new Crypt("./test/communication/rsa-keys/child.pub");
    cout << "chiPubCrypt created" << endl;
    cout << "\nAll classes created" << endl;


    /* T0 */
    cout << "\n\n\n\n########## T0: AT WEBSITE ##########" << endl;
    cout << "This is the websites policy:" << endl;
    cout << read("./test/communication/policy.data") << endl;

    cout << "Website now creates a JSON object with type Policy and the above policy as value" << endl;
    JSONHandler * jsonWeb = new JSONHandler();
    jsonWeb->put("Type", "Policy");
    jsonWeb->put( "Value", read("./test/communication/policy.data"));
    cout << "Done. This is the result:" << endl;
    cout << jsonWeb->toString() << endl;

    cout << "\nNext step is for the website to sign the json object." << endl;
    string webSign = webCrypt->sign(jsonWeb->getObject());
    cout << "Signed with webCrypt! The JSON and sign is sent to the parent" << endl;


    /* T1 */
    cout << "\n\n\n\n########### T1: AT PARENT ##########" << endl;
    cout << "The json recieved:" << endl;
    cout << jsonWeb->toString() << endl;

    cout << "\nIt verifies the signature with the websites public key (webPubCrypt)"  << endl;
    verifySignature(webPubCrypt, webSign, jsonWeb->getObject());

    cout << "\n\nWe assume the parent agrees to the policy.\
        \nThus creating the following JSON:" << endl;
    JSONHandler * jsonPar = new JSONHandler();
    string consent = chiPubCrypt->encrypt("Permission given"); // Consent is encrypted
    jsonPar->put("Type", "Consent");
    jsonPar->put("Value", consent);
    jsonPar->put("Child", "1");
    jsonPar->put("PrevSign", webSign);
    jsonPar->put("PrevJson", jsonWeb->getObject());
    cout << jsonPar->toString() << endl;

    cout << "Next the parent signs the JSON and send it back to the website" << endl;
    string parSign = parCrypt->sign(jsonPar->getObject());
    cout << "signed with parCrypt!" << endl;



    /* T2 */
    cout << "\n\n\n\n########### T2: AT WEBSITE ##########" << endl;
    cout << "The json recieved:" << endl;
    cout << jsonPar->toString() << endl;

    cout << "The website verifies the json with parents signature (parPubCrypt):" << endl;
    verifySignature(parPubCrypt, parSign, jsonPar->getObject());
    cout << "\nIt then checks the target child and forwards the JSON (with par sign)" << endl;


    /* T3 */
    cout << "\n\n\n\n########## T3: AT CHILD ##########" << endl;
    cout << "The json recieved:" << endl;
    cout << jsonPar->toString() << endl;

    cout << "\nThis is the childs data:" << endl;
    cout << read("./test/communication/child.data") << endl;

    cout << "The child verifies the json with parents signature (parPubCrypt):" << endl;
    verifySignature(parPubCrypt, parSign, jsonPar->getObject());


    cout << "\n\nIt then decrypts its message from the JSON to find that the consent is OK:" << endl;
    cout << chiCrypt->decrypt(jsonPar->get("Value")) <<  endl;


    cout << "\n\nThe child creates a new JSON and does its things, encrypting pdata, signing, etc" << endl;
    cout << "The result looks like this like this:" << endl;
    JSONHandler * jsonChi = new JSONHandler();
    string encToWeb = webPubCrypt->encrypt(read("./test/communication/child.data"));
    jsonChi->put("Type", "Pdata");
    jsonChi->put("Value", encToWeb);
    jsonChi->put("PrevSign", parSign);
    jsonChi->put("PrevJson", jsonPar->getObject());
    cout << jsonChi->toString() << endl;

    cout << "Next the child signs the JSON and send it back to the website" << endl;
    string chiSign = chiCrypt->sign(jsonChi->getObject());
    cout << "signed with chiCrypt!" << endl;


    /* T4 */
    cout << "\n\n\n\n########## T4: AT WEBSITE ##########" << endl;
    cout << "This JSON is recieved:" << endl;
    cout << jsonChi->toString() << endl;

    cout << "\nThe website verifies the json with childs signature (chiPubCrypt):" << endl;
    verifySignature(chiPubCrypt, chiSign, jsonChi->getObject());
    cout << "\n\nIt then reads the JSON and decrypts the data (webCrypt):" << endl;
    string encData = jsonChi->get("Value");
    string decData = webCrypt->decrypt(encData);
    cout << decData << endl;

    /* Recursive Lookup */
    cout << "\n\n\nDone!! For funzies, let us act as a 3rd party and recursively verify all messages sent, from the last JSON created at Child (see code for details):" << endl;


    string childSign = chiSign;
    JSON::Object::Ptr childJson = jsonChi->getObject();
    printf("%s", "Verifying Child created JSON: ");
    verifySignature(chiPubCrypt, childSign, childJson);


    string parentSign = childJson->get("PrevSign");
    JSON::Object::Ptr parentJson = childJson->getObject("PrevJson");
    printf("\n%s", "Verifying Parent created JSON: ");
    verifySignature(parPubCrypt, parentSign, parentJson);


    string websiteSign = parentJson->get("PrevSign");
    JSON::Object::Ptr websiteJson = parentJson->getObject("PrevJson");
    printf("\n%s", "Verifying Website created JSON: ");
    verifySignature(webPubCrypt, websiteSign, websiteJson);

    return 0;
}

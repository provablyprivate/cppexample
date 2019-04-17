#include <iostream>
#include <fstream>
#include "Poco/StreamCopier.h"
#include "Poco/RegularExpression.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "../../src/Crypt.cpp"
#include "../../src/JSONhandler.cpp"


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
void verifySignature(Crypt* key, string signature, JSON::Object::Ptr JSON)
{
    key->verify(JSON, signature) ? cout << "OK!" : cout << "ERROR";
}

int main() {
    for (int i = 0; i < 80; ++i) {
        printf("\n");                         // Making space
    }

    // Instantiates encryption classes for every type
    cout << "\n\nCreating crypt classes for all the types\n" << endl;
    Crypt * webCrypt = new Crypt("./test/communication/rsa-keys/website.pub", "./test/communication/rsa-keys/website");
    cout << "webCrypt    created" << endl;
    Crypt * webPubCrypt = new Crypt("./test/communication/rsa-keys/website.pub");
    cout << "webPubCrypt created" << endl;
    Crypt * parCrypt = new Crypt("./test/communication/rsa-keys/parent.pub", "./test/communication/rsa-keys/parent");
    cout << "parCrypt    created" << endl;
    Crypt * parPubCrypt = new Crypt("./test/communication/rsa-keys/parent.pub");
    cout << "parPubCrypt created" << endl;
    Crypt * chiCrypt = new Crypt("./test/communication/rsa-keys/child.pub", "./test/communication/rsa-keys/child");
    cout << "chiCrypt    created" << endl;
    Crypt * chiPubCrypt = new Crypt("./test/communication/rsa-keys/child.pub");
    cout << "chiPubCrypt created" << endl;
    cout << "\nAll classes created" << endl;


    /* T0 */
    cout << "\n\n\n\n########## T0: AT WEBSITE ##########" << endl;
    cout << "This is the websites policy:" << endl;
    cout << read("./test/communication/policy.data") << endl;

    cout << "Website now creates a JSON object with type Policy and the above policy as value" << endl;
    JSONHandler * JSONWeb = new JSONHandler();
    JSONWeb->put("Type", "Policy");
    JSONWeb->put( "Value", read("./test/communication/policy.data"));
    cout << "Done. This is the result:" << endl;
    cout << JSONWeb->toString() << endl;

    cout << "\nNext step is for the website to sign the JSON object." << endl;
    string webSign = webCrypt->sign(JSONWeb->getObject());
    cout << "Signed with webCrypt! The JSON and sign is sent to the parent" << endl;


    /* T1 */
    cout << "\n\n\n\n########### T1: AT PARENT ##########" << endl;
    cout << "The JSON recieved:" << endl;
    cout << JSONWeb->toString() << endl;

    cout << "\nIt verifies the signature with the websites public key (webPubCrypt)"  << endl;
    verifySignature(webPubCrypt, webSign, JSONWeb->getObject());

    cout << "\n\nWe assume the parent agrees to the policy.\
        \nThus creating the following JSON:" << endl;
    JSONHandler * JSONPar = new JSONHandler();
    string consent = chiPubCrypt->encrypt("Permission given"); // Consent is encrypted
    JSONPar->put("Type", "Consent");
    JSONPar->put("Value", consent);
    JSONPar->put("Child", "1");
    JSONPar->put("PrevSign", webSign);
    JSONPar->put("PrevJSON", JSONWeb->getObject());
    cout << JSONPar->toString() << endl;

    cout << "Next the parent signs the JSON and send it back to the website" << endl;
    string parSign = parCrypt->sign(JSONPar->getObject());
    cout << "signed with parCrypt!" << endl;



    /* T2 */
    cout << "\n\n\n\n########### T2: AT WEBSITE ##########" << endl;
    cout << "The JSON recieved:" << endl;
    cout << JSONPar->toString() << endl;

    cout << "The website verifies the JSON with parents signature (parPubCrypt):" << endl;
    verifySignature(parPubCrypt, parSign, JSONPar->getObject());
    cout << "\nIt then checks the target child and forwards the JSON (with par sign)" << endl;

    printf("\n********");
    cout << "\nAs a test, we will try to corrupt the JSON and verify it against the signature again. Hopefully we will get an error\n" << endl;

    JSONPar->put("Child", "1,2");
    cout << JSONPar->toString() << endl;
    verifySignature(parPubCrypt, parSign, JSONPar->getObject());
    JSONPar->put("Child", "1");
    printf("\n********");

    /* T3 */
    cout << "\n\n\n\n########## T3: AT CHILD ##########" << endl;
    cout << "The JSON recieved:" << endl;
    cout << JSONPar->toString() << endl;

    cout << "\nThis is the childs data:" << endl;
    cout << read("./test/communication/child.data") << endl;

    cout << "The child verifies the JSON with parents signature (parPubCrypt):" << endl;
    verifySignature(parPubCrypt, parSign, JSONPar->getObject());


    cout << "\n\nIt then decrypts its message from the JSON to find that the consent is OK:" << endl;
    cout << chiCrypt->decrypt(JSONPar->get("Value")) <<  endl;


    cout << "\n\nThe child creates a new JSON and does its things, encrypting pdata, signing, etc" << endl;
    cout << "The result looks like this like this:" << endl;
    JSONHandler * JSONChi = new JSONHandler();
    string encToWeb = webPubCrypt->encrypt(read("./test/communication/child.data"));
    JSONChi->put("Type", "Pdata");
    JSONChi->put("Value", encToWeb);
    JSONChi->put("PrevSign", parSign);
    JSONChi->put("PrevJSON", JSONPar->getObject());
    cout << JSONChi->toString() << endl;

    cout << "Next the child signs the JSON and send it back to the website" << endl;
    string chiSign = chiCrypt->sign(JSONChi->getObject());
    cout << "signed with chiCrypt!" << endl;


    /* T4 */
    cout << "\n\n\n\n########## T4: AT WEBSITE ##########" << endl;
    cout << "This JSON is recieved:" << endl;
    cout << JSONChi->toString() << endl;

    cout << "\nThe website verifies the JSON with childs signature (chiPubCrypt):" << endl;
    verifySignature(chiPubCrypt, chiSign, JSONChi->getObject());
    cout << "\n\nIt then reads the JSON and decrypts the data (webCrypt):" << endl;
    string encData = JSONChi->get("Value");
    string decData = webCrypt->decrypt(encData);
    cout << decData << endl;

    /* Recursive Lookup */
    cout << "\n\n\nDone!! For funzies, let us act as a 3rd party and recursively verify all messages sent, from the last JSON created at Child (see code for details):" << endl;


    string childSign = chiSign;
    JSON::Object::Ptr childJSON = JSONChi->getObject();
    printf("%s", "Verifying Child created JSON: ");
    verifySignature(chiPubCrypt, childSign, childJSON);


    string parentSign = childJSON->get("PrevSign");
    JSON::Object::Ptr parentJSON = childJSON->getObject("PrevJSON");
    printf("\n%s", "Verifying Parent created JSON: ");
    verifySignature(parPubCrypt, parentSign, parentJSON);


    string websiteSign = parentJSON->get("PrevSign");
    JSON::Object::Ptr websiteJSON = childJSON->getObject("PrevJSON")->getObject("PrevJSON");
    printf("\n%s", "Verifying Website created JSON: ");
    verifySignature(webPubCrypt, websiteSign, websiteJSON);

    return 0;
}

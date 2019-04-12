#include "./Constants.h"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../Jsonhandler.cpp"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/RegularExpression.h"
#include "Poco/StreamCopier.h"

class IParent {
 private:
    Connection *rParentConnection;
    Connection *oWebsiteConnection;
    Crypt * privateParentCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * websiteJSON;
    bool validSignature;
    Poco::Event JSONVerified = Poco::Event(true);

    std::vector<std::string> decodeHex(std::string input){
        std::string decoded;
        std::istringstream istream(input);                    // Reads the incoming message to the istream
        Poco::HexBinaryDecoder decoder(istream);              // Decodes the hex-message.
        Poco::StreamCopier::copyToString(decoder, decoded);   // Appends the decoded message it to 'decoded' variable

        std::string REGEX = "([\\s\\S]*)\n-----BEGIN SIGNATURE-----\n([\\s\\S]*)";  // creates two groups, before and after "begin signature" field
        Poco::RegularExpression re(REGEX);
        std::vector<std::string> matches;
        re.split(decoded, matches);                           // Splits the string with the above given regex

        return matches;
    }

    std::string encodeHex(JSONHandler * JSON, std::string decrypted ) {
        std::stringstream toEncode;
        JSON->getObject()->stringify(toEncode);     // Stringifies the JSON
        toEncode << "\n-----BEGIN DECRYPTED-----\n" << decrypted;  // Appends the neccessary strings


        std::ostringstream encoded;
        Poco::HexBinaryEncoder encoder(encoded);    // in parameter will be encoded to Hex
        encoder << toEncode.str();
        encoder.flush();

        return encoded.str();
    }

    void rParentConnectionHandler() {
        rParentConnection->waitForEstablishment();
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);

        while (true) {
            JSONVerified.wait();
            /*std::string encrypted = websiteJSON->get("Value");
            std::string decrypted = privateParentCrypt->decrypt(encrypted);

            std::cout << "Decrypted message: " << decrypted << std::endl;

            std::string message = encodeHex(websiteJSON, decrypted);
            rParentConnection->sendData(message);*/

            if (DEBUG) rParentConnection->sendData("Some test data from IParent");
        }
    }

 public:
    IParent(std::string websiteIP) {
        rParentConnection = new Connection(I_INTERNAL_PORT);
        oWebsiteConnection = new Connection(websiteIP, O_EXTERNAL_PORT_2);
        privateParentCrypt = new Crypt("./src/rsa-keys/parent.pub", "./src/rsa-keys/parent");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
    }

    void run() {
        Poco::RunnableAdapter<IParent> rParentFuncAdapt(*this, &IParent::rParentConnectionHandler);
        Poco::Thread rParentConnectionHandlerThread;
        rParentConnectionHandlerThread.start(rParentFuncAdapt);

        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;

            /* segfault in decodeHex()
            std::vector<std::string> messages = decodeHex(s);       // Recieves a decoded and deconstructed message

            JSONHandler * websiteJSON = new JSONHandler(messages[1]);  // creating JSON from the parsed string
            std::string websiteSignature = messages[2];               // creating signature from the parsed string

            validSignature = publicWebsiteCrypt->verify(websiteJSON->getObject(), websiteSignature);
            if (!validSignature) continue;                       // Resets the while loop if invalid signature

            // flags |= 0b10; //update flag*/
            JSONVerified.set();
        }
    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogIP.txt", "a", stdout);
    try {IParent iParent(argv[1]);
        iParent.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

#include <bitset>
#include "./Constants.h"
#include "../Crypt.cpp"
#include "../Connection.h"
#include "../Jsonhandler.cpp"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/RegularExpression.h"
#include "Poco/StreamCopier.h"

class OParent {
 private:
    Connection *rParentConnection;
    Connection *iWebsiteConnection;
    Crypt * privateParentCrypt;
    Crypt * publicChildCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * parentJSON;
    std::bitset<2> flags;
    bool validSignature;
    Poco::Event JSONUpdated = Poco::Event(true);

    bool isJSON(std::string input) {
        std::string separator = "-----BEGIN SIGNATURE-----";
        return (input.find(separator) != std::string::npos);
    }

    std::vector<std::string> decodeHex(std::string input) {
        std::string decoded;
        std::istringstream istream(input);                    // Reads the incoming message to the istream
        Poco::HexBinaryDecoder decoder(istream);              // Decodes the hex-message.
        Poco::StreamCopier::copyToString(decoder, decoded);   // Appends the decoded message it to 'decoded' variable
        std::vector<std::string> matches;

        if (isJSON(decoded)) {
            std::string REGEX = "([\\s\\S]*)\n-----BEGIN SIGNATURE-----\n([\\s\\S]*)"; // creates two groups, before and after "begin signature" field
            Poco::RegularExpression re(REGEX);
            re.split(decoded, matches);                  // Splits the string with the above given regexa
        } else {
            matches[1] = decoded;
        }

        return matches;
    }

    std::string encodeHex(JSONHandler * JSON, std::string Signature) {
        std::stringstream toEncode;
        JSON->getObject()->stringify(toEncode);                 // Stringifies the JSON
        toEncode << "\n-----BEGIN SIGNATURE-----\n" << Signature; // Appends the neccessary strings

        std::ostringstream encoded;
        Poco::HexBinaryEncoder encoder(encoded);                // in parameter will be encoded to Hex
        encoder << toEncode.str();
        encoder.flush();

        return encoded.str();
    }

    void rParentConnectionHandler() {
        rParentConnection->waitForEstablishment();
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);

        std::string s;
        while (true) {
            rParentConnection->waitForReceivedData();
            s = rParentConnection->getData();
            std::cout << "Received from RChild: " << s << std::endl;

            std::vector<std::string> messages = decodeHex(s);       // Recieves a decoded and deconstructed message

            if (sizeof(messages[2]) > 0) {  // if it's a json. BAD???
                JSONHandler * previousJSON = new JSONHandler(messages[1]); // creating JSON from the parsed string
                std::string previousSignature = messages[2];               // creating signature from the parsed string

                validSignature = publicWebsiteCrypt->verify(previousJSON->getObject(), previousSignature);
                if (not validSignature) continue;                       // Resets the while loop if invalid signature

                parentJSON->put("PrevJson", previousJSON->getObject());  // Get the object and puts it in the JSON
                parentJSON->put("PrevSign", previousSignature);          // Puts the signature
                flags |= 0b10; //update flag
                JSONUpdated.set();

            } else if (sizeof(messages[1]) > 0) {
                std::string consent = messages[1];
                std::string encryptedData = privateParentCrypt->encrypt(consent);

                parentJSON->put("Value", encryptedData);
                flags |= 0b01; //update flag
                JSONUpdated.set();
            }
        }
    }

 public:
    OParent(std::string websiteIP) {
        rParentConnection = new Connection(O_INTERNAL_PORT);
        //iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_1);
        //oWebsiteConnection = new Connection(O_EXTERNAL_PORT_1);

        privateParentCrypt = new Crypt("./src/rsa-keys/parent.pub","./src/rsa-keys/parent");
        publicChildCrypt = new Crypt("./src/rsa-keys/child.pub");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
        parentJSON = new JSONHandler();
        parentJSON->put("Type", "Consent");  // This might now follow type system!!!!
        flags = 0b00;
    }

    void run() {

        Poco::RunnableAdapter<OParent> rParentFuncAdapt(*this, &OParent::rParentConnectionHandler);
        Poco::Thread rParentConnectionHandlerThread;
        rParentConnectionHandlerThread.start(rParentFuncAdapt);

        /*Poco::RunnableAdapter<OParent> iWebsiteFuncAdapt(*this, &OParent::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);*/

        /*Poco::RunnableAdapter<OParent> oWebsiteFuncAdapt(*this, &OParent::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);

        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);*/

        while (true) {
            JSONUpdated.wait();
            if (flags.all()) {
                std::string signature = privateParentCrypt->sign(parentJSON->getObject());
                std::string message = encodeHex(parentJSON, signature);
                iWebsiteConnection->sendData(message);
                flags = 0b00;
            } else {
                continue;
            }
        }
    }
};

int main(int argc, char **argv) {
    try {OParent oParent(argv[1]);
        oParent.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

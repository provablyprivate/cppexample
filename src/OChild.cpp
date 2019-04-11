/*
 * MISSING
 * function to send messate to Iwebsite
 */

#include "Constants.h"
#include "Connection.h"
#include "./crypt.cpp"
#include "./jsonhandler.cpp"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/RegularExpression.h"
#include "Poco/StreamCopier.h"
#include <bitset>

class OChild {

private:
    Connection *rChildConnection;
    Connection *iWebsiteConnection;
    Connection *oWebsiteConnection;
    Crypt * privateChildCrypt;
    Crypt * publicParentCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * childJSON;
    std::bitset<2> flags;
    bool validSignature;
    Poco::Event JSONUpdated = Poco::Event(true);

    std::vector<std::string> decodeHex(std::string input){
        std::string decoded;
        std::istringstream istream(input);                    // Reads the incoming message to the istream
        Poco::HexBinaryDecoder decoder(istream);              // Decodes the hex-message.
        Poco::StreamCopier::copyToString(decoder, decoded);   // Appends the decoded message it to 'decoded' variable

        std::string REGEX = "([\\s\\S]*)\n----BEGIN SIGNATURE----\n([\\s\\S]*)"; // creates two groups, before and after "begin signature" field
        Poco::RegularExpression re (REGEX);
        std::vector<std::string> matches;
        re.split(decoded, matches);                           // Splits the string with the above given regex

        return matches;
    }

    std::string encodeHex(JSONHandler * JSON, std::string Signature){
        std::stringstream toEncode;
        JSON->getObject()->stringify(toEncode);                 // Stringifies the JSON
        toEncode << "\n----BEGIN SIGNATURE----\n" << Signature; // Appends the neccessary strings

        std::ostringstream encoded;
        Poco::HexBinaryEncoder encoder(encoded);                // in parameter will be encoded to Hex
        encoder << toEncode.str();
        encoder.flush();

        return encoded.str();
    }

    void oWebsiteConnectionHandler() {
        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;

            std::vector<std::string> messages = decodeHex(s);       // Recieves a decoded and deconstructed message

            JSONHandler * previousJSON = new JSONHandler(messages[1]); // creating JSON from the parsed string
            std::string previousSignature = messages[2];               // creating signature from the parsed string

            validSignature = publicParentCrypt->verify(previousJSON->getObject(), previousSignature);
            if (not validSignature) continue;                       // Resets the while loop if invalid signature

            childJSON->put("PrevJson", previousJSON->getObject());  // Get the object and puts it in the JSON
            childJSON->put("PrevSign", previousSignature);          // Puts the signature

            flags |= 0b10; //update flag
            //*notify thread*
        }
    }

    void rChildConnectionHandler() {
        rChildConnection->waitForEstablishment();
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);

        std::string s;
        while (true) {
            rChildConnection->waitForReceivedData();
            s = rChildConnection->getData();
            std::cout << "Received from RChild: " << s << std::endl;

            string encryptedData = publicWebsiteCrypt->encrypt(s);  // encrypts the data
            childJSON->put("Value", encryptedData);                 // Puts it in the JSON

            flags |= 0b01; //update flag
            //*notify thread*
            JSONUpdated.set();
        }
    }

public:
    OChild(std::string websiteIP) {
        rChildConnection = new Connection(O_INTERNAL_PORT);
        //iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_1);
        //oWebsiteConnection = new Connection(O_EXTERNAL_PORT_1);
        
        privateChildCrypt = new Crypt("./src/rsa-keys/child.pub", "./src/rsa-keys/child");
        publicParentCrypt = new Crypt("./src/rsa-keys/parent.pub");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
        childJSON = new JSONHandler();
        flags = 0b00;
    }

    void run() {

        Poco::RunnableAdapter<OChild> rChildFuncAdapt(*this, &OChild::rChildConnectionHandler);
        Poco::Thread rChildConnectionHandlerThread;
        rChildConnectionHandlerThread.start(rChildFuncAdapt);

        /*Poco::RunnableAdapter<OChild> iWebsiteFuncAdapt(*this, &OChild::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);*/

        /*Poco::RunnableAdapter<OChild> oWebsiteFuncAdapt(*this, &OChild::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);

        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);*/
        
        while (true) {
            JSONUpdated.wait();
            //check flags, send if done
            
        }
    }
};

int main(int argc, char **argv) {
    try {OChild oChild(argv[1]);
        oChild.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

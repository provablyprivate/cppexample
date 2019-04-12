/*
 *TODO: Should the child accumulate messages into a buffer, and send all once a consent is received?
 *
 * How the created JSON of the Child will look like:
 *
 * {
 *  "Type":  "PDATA",
 *  "Value": "*ENCRYPTED*",
 *  "Previous: {
 *      "Json": *Parent JSON*
 *      "Signature: *Parent Signature*
 *      }
 * }
 */

#include <bitset>
#include "./Constants.h"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../Jsonhandler.cpp"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/RegularExpression.h"
#include "Poco/StreamCopier.h"

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

    /*! \brief Decodes a hex string.
     *
     * Decodes a hex string and returns the result.
     *
     * \return std::string decoded
     */
    std::string decodeHex(std::string input) {
        std::string decoded;
        std::istringstream istream(input);
        Poco::HexBinaryDecoder decoder(istream);
        Poco::StreamCopier::copyToString(decoder, decoded);

        return decoded;
    }

    /*! \brief Splits string given a delimiter
     *
     * Used to split the incoming hex messages separated by a dot character.
     *
     * \return std::vector<std::string> matches
     */
    std::vector<std::string> splitString(std::string input, char delimeter) {
        std::istringstream ss(input);
        std::string match;
        std::vector<std::string> matches;
        while (std::getline(ss, match, delimeter)) {
           matches.push_back(match);
        }

        return matches;
    }

    /*! \brief Relays the data to the recipient
     *
     *  JSONUpdated.wait() makes the function sleep until one of the threads signal an
     *  update to the Json object. When woken up, the function will check if all flags are set,
     *  i.e. that every thread has completed its computation. If not, the loop is reset.
     *
     *  If all flags are set the constructed message will have the format "jsonHex.signatureHex".
     *  Once the message is sent, the global 'flags' variable is reset.
     */
    void relayData() {
        while (true) {
            JSONUpdated.wait();
            if (flags.all()) {
                std::string jsonHex = childJSON->toHex();
                std::string signatureHex = privateChildCrypt->sign(childJSON->getObject());
                std::string message = jsonHex + "." + signatureHex;
                std::cout << "Sending to iWebsite: " << message << std::endl;
                iWebsiteConnection->sendData(message);
                flags = 0b00;
            } else {
                if (DEBUG) iWebsiteConnection->sendData("Some test data from OChild");
                continue;
            }
        }
    }

    /*! \brief Connection handler for the OWebsite interface
     *
     * Creates a new thread which sleeps until a message from OWebsite is received.
     * When woken up, the message is split up into two parts. The first parts gets decoded
     * and parsed as the websites JSON object. The second part is its signature.
     * The JSON and signature are verified against each other using a Crypt instance
     * created with the Websites public RSA key. If the signature is not valid the loop
     * will reset.
     *
     * If the signature is valid, a new JSON object called 'Previous' will be created.
     * Both the received JSON and signature will be inserted into 'Previous',
     * for future recursive look up. The MSB in 'flags' will be set to 1 to indicate
     * proper execution. Finally the function will wake relayData() using JSONUpdated.set().
     */
    void oWebsiteConnectionHandler() {
        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;

            // It is known that the recieved message will be of format JsonHEX.SignatureHEX
            // Hence we can 'hard code' which hex needs to be decoded or not.
            std::vector<std::string> messages = splitString(s, '.');
            JSONHandler * previousJSON = new JSONHandler(decodeHex(messages[0]));
            std::string previousSignature = messages[1];

            validSignature = publicParentCrypt->verify(previousJSON->getObject(), previousSignature);
            if (!validSignature) continue;

            JSONHandler * previous = new JSONHandler();
            previous->put("Json", previousJSON->getObject());
            previous->put("Signature", previousSignature);
            childJSON->put("Previous", previous->getObject());

            flags |= 0b10;
            JSONUpdated.set();
        }
    }

    /*! \brief Connection handler for the RChild interface
     *
     * Creates a new thread which sleeps until a message from RChild is received.
     * When woken up, the message is encrypted using a Crypt instance created with
     * the Child's private RSA key. The message will be inserted into the agents JSON.
     * The LSB in 'flags' will be set to 1 to indicate proper execution.
     * Finally the function will wake relayData() using JSONUpdated.set().
     *
     * The loop then resets, ensuring that the Child's messages are always accepted
     * even though no consent is given.
     */
    void rChildConnectionHandler() {
        rChildConnection->waitForEstablishment();
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);

        std::string s;
        while (true) {
            rChildConnection->waitForReceivedData();
            s = rChildConnection->getData();
            std::cout << "Received from RChild: " << s << std::endl;

            string encryptedData = publicWebsiteCrypt->encrypt(s);
            childJSON->put("Value", encryptedData);

            flags |= 0b01;
            JSONUpdated.set();
        }
    }

 public:
    OChild(std::string websiteIP) {
        rChildConnection   = new Connection(O_INTERNAL_PORT);
        iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_1);
        oWebsiteConnection = new Connection(websiteIP, O_EXTERNAL_PORT_1);

        privateChildCrypt  = new Crypt("./src/rsa-keys/child.pub", "./src/rsa-keys/child");
        publicParentCrypt  = new Crypt("./src/rsa-keys/parent.pub");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");

        childJSON          = new JSONHandler();
        childJSON->put("Type", "PDATA");  // This might not follow type system! Unclear!!

        flags = 0b00;
    }

    /*! \brief Main function run by the class
     *
     * Each interface connected to will have its own thread created for parallel computation.
     */
    void run() {
        Poco::RunnableAdapter<OChild> rChildFuncAdapt(*this, &OChild::rChildConnectionHandler);
        Poco::Thread rChildConnectionHandlerThread;
        rChildConnectionHandlerThread.start(rChildFuncAdapt);

        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        Poco::RunnableAdapter<OChild> oWebsiteFuncAdapt(*this, &OChild::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);

        relayData();
    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogOC.txt", "a", stdout);
    try {OChild oChild(argv[1]);
        oChild.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

class OChild {
 private:
    Connection      * rChildConnection,  * iWebsiteConnection, * oWebsiteConnection;
    Crypt           * privateChildCrypt, * publicParentCrypt,  * publicWebsiteCrypt;
    InterfaceHelper * helper;
    JSONHandler     * childJSON;
    bool validSignature;
    Poco::Event JSONUpdated = Poco::Event(true);

    /*! \brief Relays the data to the recipient
     *
     *  JSONUpdated.wait() makes the function sleep until one of the threads signal an
     *  update to the JSON object. When woken up, the function will check if all flags are set,
     *  i.e. that every thread has completed its computation. If not, the loop is reset.
     *
     *  If all flags are set the constructed message will have the format "JSONHex.signatureHex".
     *  Once the message is sent, the global 'flags' variable is reset.
     */
    void relayData() {
        iWebsiteConnection->waitForEstablishment();
        while (true) {
            JSONUpdated.wait();
            if (helper->all()) {
                std::string JSONHex = childJSON->toHex();
                std::string signatureHex = privateChildCrypt->sign(childJSON->getObject());
                std::string message = JSONHex + "." + signatureHex;
                std::cout << "Sending to iWebsite: " << message << std::endl;
                iWebsiteConnection->sendData(message);
                helper->clear();
            } else {
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
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);
        oWebsiteConnection->waitForEstablishment();

        std::string incoming;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            incoming = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << incoming << std::endl;

            // It is known that the recieved message will be of format JSONHEX.SignatureHEX
            // Hence we can 'hard code' which hex needs to be decoded or not.
            std::vector<std::string> messages = helper->splitString(incoming, '.');
            JSONHandler * previousJSON = new JSONHandler(helper->decodeHex(messages[0]));
            std::string previousSignature = messages[1];

            validSignature = publicParentCrypt->verify(previousJSON->getObject(), previousSignature);
            if (!validSignature) continue;

            JSONHandler * previous = new JSONHandler();
            previous->put("JSON", previousJSON->getObject());
            previous->put("Signature", previousSignature);
            childJSON->put("Previous", previous->getObject());

            helper->set(1, true);
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
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);
        rChildConnection->waitForEstablishment();

        std::string incoming;
        while (true) {
            rChildConnection->waitForReceivedData();
            incoming = rChildConnection->getData();
            std::cout << "Received from RChild: " << incoming << std::endl;

            string encryptedData = publicWebsiteCrypt->encrypt(incoming);
            childJSON->put("Value", encryptedData);

            helper->set(0, true);
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

        helper             = new InterfaceHelper(2);
        childJSON          = new JSONHandler();
        childJSON->put("Type", "PDATA");
    }

    /*! \brief Main function run by the class
     *
     * Each interface connected to will have its own thread created for parallel computation.
     */
    void run() {
        Poco::RunnableAdapter<OChild> rChildFuncAdapt(*this, &OChild::rChildConnectionHandler);
        Poco::Thread rChildConnectionHandlerThread;
        rChildConnectionHandlerThread.start(rChildFuncAdapt);

        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        Poco::RunnableAdapter<OChild> oWebsiteFuncAdapt(*this, &OChild::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);

        relayData();
    }
};

int main(int argc, char **argv) {
    OChild oChild(argv[1]);
    oChild.run();
        
    return 0;
}

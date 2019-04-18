#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/StreamCopier.h"

class IParent {
 private:
    Connection      * rParentConnection,  * oWebsiteConnection;
    Crypt           * privateParentCrypt, * publicWebsiteCrypt;
    InterfaceHelper * helper;
    JSONHandler     * websiteJSON;
    std::string recievedMessage;
    bool validSignature;
    Poco::Event JSONVerified = Poco::Event(true);

    /* Decodes the hex containing the websites encrypted message.
     * Decrypts this message, encodes it back to hex and replaces
     * the encrypted message with the decrypted one. The new message is
     * sent to Parent via RParent.
     */
    void rParentConnectionHandler() {
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);
        rParentConnection->waitForEstablishment();

        while (true) {
            JSONVerified.wait();
            std::string encrypted = websiteJSON->get("Value");
            std::string decrypted = privateParentCrypt->decrypt(encrypted);

            std::cout << "Decrypted message: " << decrypted << std::endl;

            std::string message = recievedMessage + "." + helper->encodeHex(decrypted);
            rParentConnection->sendData(message);
        }
    }

 public:
    IParent(std::string websiteIP) {
        rParentConnection  = new Connection(I_INTERNAL_PORT);
        oWebsiteConnection = new Connection(websiteIP, O_EXTERNAL_PORT_2);
        privateParentCrypt = new Crypt("./src/rsa-keys/parent.pub", "./src/rsa-keys/parent");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
        helper             = new InterfaceHelper();
    }

    void run() {
        Poco::RunnableAdapter<IParent> rParentFuncAdapt(*this, &IParent::rParentConnectionHandler);
        Poco::Thread rParentConnectionHandlerThread;
        rParentConnectionHandlerThread.start(rParentFuncAdapt);

        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);
        oWebsiteConnection->waitForEstablishment();

        /* Decodes the incoming hex messages in order to verify
         * the JSON against the signature. If the signature isn't valid,
         * the loop resets. Otherwise, rParentConnectionHandler() is awoken.
         */
        std::string incoming;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            incoming = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << incoming << std::endl;

            std::vector<std::string> messages = helper->splitString(incoming, '.');

            if (messages.size() != 2)
                continue;

            websiteJSON = new JSONHandler(helper->decodeHex(messages[0]));
            std::string websiteSignature = messages[1];
            validSignature = publicWebsiteCrypt->verify(websiteJSON->getObject(), websiteSignature);

            if (!validSignature)
                continue;

            recievedMessage = incoming;
            JSONVerified.set();
        }
    }
};

int main(int argc, char **argv) {
    freopen("./errorlogIP.txt", "a", stdout);
    try {IParent iParent(argv[1]);
        iParent.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

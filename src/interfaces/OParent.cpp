#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

class OParent {
 private:
    Connection      * iWebsiteConnection, * rParentConnection;
    Crypt           * privateParentCrypt, * publicChildCrypt,  * publicWebsiteCrypt;
    InterfaceHelper * helper;
    JSONHandler     * parentJSON;
    Poco::Event JSONUpdated = Poco::Event(true);

    /* Creates the message sent to IWebsite, ie the parents JSON and
     * its signature encoded as a dot separated hex message.
     *
     * Note that this is only done when all flags are set to 1, ie when
     * every threaded process has completed its task.
     */
    void relayData() {
        iWebsiteConnection->waitForEstablishment();
        while (true) {
            JSONUpdated.wait();
            if (helper->all()) {
                std::string JSONHex = parentJSON->toHex();
                std::string signatureHex = privateParentCrypt->sign(parentJSON->getObject());
                std::string message = JSONHex + "." + signatureHex;
                std::cout << "Sending to iWebsite: " << helper->decodeHex(message) << std::endl;
                iWebsiteConnection->sendData(message);
                helper->clear();
            } else {
                continue;
            }
        }
    }

    /* Sets the previous JSON and signature in a separate field OR
     * encrypts and sets parents consent reponse in the Value field.
     * The cases are distinguished by looking at the size of the message.
     * The former will contain two dot separated messages, whereas the latter
     * will be a single message. Both processes signal that the JSON has been updated.
     * */
    void rParentConnectionHandler() {
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);
        rParentConnection->waitForEstablishment();

        std::string incoming;
        while (true) {
            rParentConnection->waitForReceivedData();
            incoming = rParentConnection->getData();
            std::vector<std::string> messages = helper->splitString(incoming, '.');
            
            std::cout << "Received from RParent: " << std::endl; for (int i = 0; i < messages.size(); i++) { std::cout << helper->decodeHex(messages[i]) << std::endl; }

            if (messages.size() > 1) {
                JSONHandler * previousJSON = new JSONHandler(helper->decodeHex(messages[0]));
                std::string previousSignature = messages[1];

                JSONHandler * previous = new JSONHandler();
                previous->put("JSON", previousJSON->getObject());
                previous->put("Signature", previousSignature);
                parentJSON->put("Previous", previous->getObject());

                helper->set(1,true);
                JSONUpdated.set();

            } else if (messages.size() > 0) {
                std::string consent = messages[0];
                std::string encryptedData = publicWebsiteCrypt->encrypt(consent);


                parentJSON->put("Value", encryptedData);
                helper->set(0,true);
                JSONUpdated.set();
            }
        }
    }

 public:
    OParent(std::string websiteIP) {
        rParentConnection  = new Connection(O_INTERNAL_PORT);
        iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_2);
        helper             = new InterfaceHelper(2);
        privateParentCrypt = new Crypt("./src/rsa-keys/parent.pub", "./src/rsa-keys/parent");
        publicChildCrypt   = new Crypt("./src/rsa-keys/child.pub");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
        parentJSON         = new JSONHandler();
        parentJSON->put("Type", "CwebsiteCONSENT");  // This might not follow type system!!!!
    }

    void run() {
        Poco::RunnableAdapter<OParent> rParentFuncAdapt(*this, &OParent::rParentConnectionHandler);
        Poco::Thread rParentConnectionHandlerThread;
        rParentConnectionHandlerThread.start(rParentFuncAdapt);

        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        relayData();
    }
};

int main(int argc, char **argv) {
    OParent oParent(argv[1]);
    oParent.run();

    return 0;
}

#include <bitset>
#include <stdlib.h>
#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../Jsonhandler.cpp"

class OParent {
 private:
    Connection *rParentConnection;
    Connection *iWebsiteConnection;
    Crypt * privateParentCrypt;
    Crypt * publicChildCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * parentJSON;
    InterfaceHelper * helper;
    Poco::Event JSONUpdated = Poco::Event(true);

    //identical to OChild. only thing different is class json and crypt. generlize?
    void relayData() {
        while (true) {
            JSONUpdated.wait();
            if (helper->all()) {
                std::string jsonHex = parentJSON->toHex();
                std::string signatureHex = privateParentCrypt->sign(parentJSON->getObject());
                std::string message = jsonHex + "." + signatureHex;
                std::cout << "Sending to iWebsite: " << message << std::endl;
                iWebsiteConnection->sendData(message);
                helper->clear();
            } else {
                if (DEBUG) iWebsiteConnection->sendData("Some test data from OChild");
                continue;
            }
        }
    }

    void rParentConnectionHandler() {
        rParentConnection->waitForEstablishment();
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);

        std::string s;
        while (true) {
            rParentConnection->waitForReceivedData();
            s = rParentConnection->getData();
            std::cout << "Received from RParent: " << s << std::endl;

            if (DEBUG) { std::cout << "Sending it to IWebsite" << std::endl; iWebsiteConnection->sendData(s); }

            //The following is different from OChild since incoming messages can be of two types
            //But it essentially works like Owebsite and Rchild in one function
            //Difference is that we don't verify signature here since IParent already does that
            //If we do it anyway we can generlize function??

            std::vector<std::string> messages = helper->splitString(s, '.');

            if (messages.size() > 1) {
                JSONHandler * previousJSON = new JSONHandler(helper->decodeHex(messages[0]));
                std::string previousSignature = messages[1];

                // validSignature = publicWebsiteCrypt->verify(previousJSON->getObject(), previousSignature);
                // if (!validSignature) continue;

                JSONHandler * previous = new JSONHandler();
                previous->put("Json", previousJSON->getObject());
                previous->put("Signature", previousSignature);
                parentJSON->put("Previous", previous->getObject());

                helper->set(1,true);
                JSONUpdated.set();

            } else if (messages.size() > 0) {
                std::string consent = messages[1]; // Encrypted consent insttead of s, like Ochild
                std::string encryptedData = privateParentCrypt->encrypt(consent);

                parentJSON->put("Value", encryptedData);
                helper->set(0,true);
                JSONUpdated.set();
            }
        }
    }

 public:
    //All of this is almost identical to Ochild
    OParent(std::string websiteIP) {
        rParentConnection  = new Connection(O_INTERNAL_PORT);
        iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_2);

        helper             = new InterfaceHelper(2);

        privateParentCrypt = new Crypt("./src/rsa-keys/parent.pub","./src/rsa-keys/parent");
        publicChildCrypt   = new Crypt("./src/rsa-keys/child.pub");
        publicWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub");
        parentJSON         = new JSONHandler();
        parentJSON->put("Type", "Consent");  // This might not follow type system!!!!
    }

    void run() {
        Poco::RunnableAdapter<OParent> rParentFuncAdapt(*this, &OParent::rParentConnectionHandler);
        Poco::Thread rParentConnectionHandlerThread;
        rParentConnectionHandlerThread.start(rParentFuncAdapt);

        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        if (DEBUG) { while (true) { sleep(10); iWebsiteConnection->sendData("Test data from OParent"); } }

        relayData();
    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogOP.txt", "a", stdout);
    try {OParent oParent(argv[1]);
        oParent.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

#include "./Constants.h"
#include "../Connection.h"
#include "./InterfaceHelper.cpp"
#include "../JSONhandler.cpp"
#include "../Crypt.cpp"


class IWebsite {
private:
    Connection      * rWebsiteConnection,  * oChildConnection,   * oParentConnection;
    Crypt           * privateWebsiteCrypt, * publicParentCrypt, * publicChildCrypt;
    InterfaceHelper * helper;
    JSONHandler     * parentJSON, * childJSON;
    bool validParentSignature, validChildSignature;

    void oChildConnectionHandler() {
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        rWebsiteConnection->waitForEstablishment();
        oChildConnection->waitForEstablishment();

        std::string incoming;
        while (true) {
            oChildConnection->waitForReceivedData();
            std::vector<std::string> messages = helper->splitString(incoming, '.');

            if (messages.size() != 2)
                continue;

            childJSON = new JSONHandler(helper->decodeHex(messages[0]));
            std::string childSignature = helper->decodeHex(messages[1]);
            validChildSignature = publicChildCrypt->verify(childJSON->getObject(), childSignature);

            if (!validChildSignature)
                continue;

            // Decrypt pdata, pass to Website (via RWebsite)
            std::string privateData = privateWebsiteCrypt->decrypt(childJSON->get("Value"));
            rWebsiteConnection->sendData(privateData);

        }
    }

public:
    IWebsite() {
        rWebsiteConnection = new Connection(I_INTERNAL_PORT);
        oChildConnection = new Connection(I_EXTERNAL_PORT_1);
        oParentConnection = new Connection(I_EXTERNAL_PORT_2);
        helper = new InterfaceHelper();
        privateWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub", "./src/rsa-keys/website");
        publicParentCrypt = new Crypt("./src/rsa-keys/parent.pub");
        publicChildCrypt = new Crypt("./src/rsa-keys/child.pub");

    }

    void run() {
        Poco::Thread rWebsiteConnectionThread;
        rWebsiteConnectionThread.start(*rWebsiteConnection);

        Poco::RunnableAdapter<IWebsite> oChildFuncAdapt(*this, &IWebsite::oChildConnectionHandler);
        Poco::Thread oChildConnectionHandlerThread;
        oChildConnectionHandlerThread.start(oChildFuncAdapt);

        Poco::Thread oParentConnectionThread;
        oParentConnectionThread.start(*oParentConnection);

        rWebsiteConnection->waitForEstablishment();
        oParentConnection->waitForEstablishment();

        std::string incoming; // consent goes here
        while (true) {
            oParentConnection->waitForReceivedData();
            incoming = oParentConnection->getData();

            std::vector<std::string> messages = helper->splitString(incoming, 's');

            if (messages.size() != 2)
                continue;

            parentJSON = new JSONHandler(helper->decodeHex(messages[0]));
            std::string parentSignature = helper->decodeHex(messages[1]);

            validParentSignature = publicParentCrypt->verify(parentJSON->getObject(), parentSignature);

            if (!validParentSignature)
                continue;

            rWebsiteConnection->sendData(incoming);
            // decrypt, verify signature etc, pass to RWebsite


        }
    }


};

int main() {
    //if (DEBUG) freopen("./errorlogIW.txt", "a", stdout);
    IWebsite iWebsite;
    iWebsite.run();
}

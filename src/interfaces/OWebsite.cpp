#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

class OWebsite {
 private:
    Connection      * iParentConnection,   * oChildConnection, * rWebsiteConnection;
    Crypt           * privateWebsiteCrypt, * publicChildCrypt, * publicParentCrypt;
    InterfaceHelper * helper;
    JSONHandler     * websiteJSON;

 public:
    OWebsite() {
        rWebsiteConnection  = new Connection(O_INTERNAL_PORT);
        oChildConnection    = new Connection(O_EXTERNAL_PORT_1);
        iParentConnection   = new Connection(O_EXTERNAL_PORT_2);
        helper              = new InterfaceHelper();
        privateWebsiteCrypt = new Crypt("./src/rsa-keys/website.pub", "./src/rsa-keys/website");
        publicChildCrypt    = new Crypt("./src/rsa-keys/child.pub");
        publicParentCrypt   = new Crypt("./src/rsa-keys/parent.pub");
        websiteJSON         = new JSONHandler();

        websiteJSON->put("Type", "Consent");
    }

    void run() {
        Poco::Thread rWebsiteConnectionThread;
        rWebsiteConnectionThread.start(*rWebsiteConnection);

        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        Poco::Thread iParentConnectionThread;
        iParentConnectionThread.start(*iParentConnection);

        rWebsiteConnection->waitForEstablishment();
        oChildConnection->waitForEstablishment();
        iParentConnection->waitForEstablishment();

        /* Sending messages either to IParent or OChild. Messages of structure
         * 'HEX.HEX' will be sent to OChild since this is the parent consent with its
         * signed JSON. A single hex encoded message is the websites policy. Hence the plain text
         * policy will be put in the websites JSON, signed, encoded and sent to iParent.
         */
        std::string incoming;
        while (true) {
            rWebsiteConnection->waitForReceivedData();
            incoming  = rWebsiteConnection->getData();
            std::cout << "Received from RWebsite: " << incoming << std::endl;

            std::vector<std::string> messages = helper->splitString(incoming, '.');

            if (messages.size() > 1) {
                std::cout << "Forwarding to oChild" << std::endl;
                oChildConnection->sendData(incoming);

            } else if (messages.size() > 0) {
                std::string policy = messages[0];
                websiteJSON->put("Value", publicParentCrypt->encrypt(helper->decodeHex(policy)));
                std::string JSONHex = websiteJSON->toHex();
                std::string signatureHex = privateWebsiteCrypt->sign(websiteJSON->getObject());
                std::string message = JSONHex + '.' + signatureHex;
                std::cout << "Forwarding to iParent" << std::endl;
                iParentConnection->sendData(message);

            } else {
                continue;
            }
        }
    }
};

int main() {
    OWebsite oWebsite;
    oWebsite.run();
}

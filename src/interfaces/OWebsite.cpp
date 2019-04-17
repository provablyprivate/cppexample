#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../Jsonhandler.cpp"

class OWebsite {
 private:
    Connection      * iParentConnection;
    Connection      * oChildConnection;
    Connection      * rWebsiteConnection;
    Crypt           * privateWebsiteCrypt;
    Crypt           * publicChildCrypt;
    Crypt           * publicParentCrypt;
    InterfaceHelper * helper;
    JSONHandler     * websiteJSON;

 public:
    OWebsite() {
        rWebsiteConnection  = new Connection(O_INTERNAL_PORT);
        oChildConnection    = new Connection(O_EXTERNAL_PORT_1);
        iParentConnection   = new Connection(O_EXTERNAL_PORT_2);
        helper              = new InterfaceHelper();
        privateWebsiteCrypt = new Crypt("/src/rsa-keys/website.pub", "./src/rsa-keys/website");
        publicChildCrypt    = new Crypt("./src/rsa-keys(child.pub");
        publicParentCrypt   = new Crypt("./src/rsa-keys(parent.pub");
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

        std::string s;
        while (true) {
            rWebsiteConnection->waitForReceivedData();
            s = rWebsiteConnection->getData();
            std::cout << "Received from RWebsite: " << s << std::endl;

            std::vector<std::string> messages = helper->splitString(s, '.');

            if (messages.size() > 1) {
                std::cout << "Forwarding to iParent" << std::endl;
                oChildConnection->sendData(s);

            } else if (messages.size() > 0) {
                std::string policy = messages[0];
                websiteJSON->put("Value", policy);
                std::string jsonHex = websiteJSON->toHex();
                std::string signatureHex = privateWebsiteCrypt->sign(websiteJSON->getObject());
                std::string message = jsonHex + '.' + signatureHex;
                iParentConnection->sendData(message);

            } else {
                if (DEBUG) { std::cout << "Sending it to IParent" << std::endl; iParentConnection->sendData(s); }
                continue;
            }
        }
    }
};

int main() {
    //if (DEBUG) freopen("./errorlogOW.txt", "a", stdout);
    OWebsite oWebsite;
    oWebsite.run();
}

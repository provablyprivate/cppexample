#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../Jsonhandler.cpp"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/StreamCopier.h"

class IParent {
 private:
    Connection *rParentConnection;
    Connection *oWebsiteConnection;
    Crypt * privateParentCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * websiteJSON;
    std::string recievedMessage;
    InterfaceHelper * helper;
    bool validSignature;
    Poco::Event JSONVerified = Poco::Event(true);

    void rParentConnectionHandler() {
        rParentConnection->waitForEstablishment();
        Poco::Thread rParentConnectionThread;
        rParentConnectionThread.start(*rParentConnection);

        while (true) {
            JSONVerified.wait();
            std::string encrypted = websiteJSON->get("Value");
            std::string decrypted = privateParentCrypt->decrypt(encrypted);

            std::cout << "Decrypted message: " << decrypted << std::endl;

            std::string message = recievedMessage + "." + helper->encodeHex(decrypted);
            rParentConnection->sendData(message);

            if (DEBUG) rParentConnection->sendData("Some test data from IParent");
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

        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        //Put in own function??
        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;

            std::vector<std::string> messages = helper->splitString(s, '.');
            websiteJSON = new JSONHandler(messages[0]);
            std::string websiteSignature = messages[1];

            validSignature = publicWebsiteCrypt->verify(websiteJSON->getObject(), websiteSignature);
            if (!validSignature) continue;

            //Different. we need this to be global. or????
            recievedMessage = s;
            JSONVerified.set();
        }
    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogIP.txt", "a", stdout);
    try {IParent iParent(argv[1]);
        iParent.run();
    }
    catch (Poco::FileException e) {
        std::cout << e.what();
    }
    return 0;
}

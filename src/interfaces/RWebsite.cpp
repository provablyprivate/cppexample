#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

class RWebsite {
 private:
    Connection      * iWebsiteConnection, * oWebsiteConnection, * websiteParentConnection, * websiteChildConnection;
    InterfaceHelper * helper;

    void iWebsiteConnectionHandler() {
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        iWebsiteConnection->waitForEstablishment();
        oWebsiteConnection->waitForEstablishment();
        websiteChildConnection->waitForEstablishment();
        websiteParentConnection->waitForEstablishment();

        /* Delegates the messages depending on if it came from Parent or Child.
         * Messages from Parents will be directly forwarded to OParent in order
         * to be sent to Child. Messages from child will be decoded and sent in
         * plain text to the Website.
         */
        std::string incoming;
        while (true) {
            iWebsiteConnection->waitForReceivedData();
            incoming = iWebsiteConnection->getData();
            std::vector<std::string> messages = helper->splitString(incoming, '.');
            
            std::cout << "Received from iWebsite: " << std::endl; for (int i = 0; i < messages.size(); i++) { std::cout << helper->decodeHex(messages[i]) << std::endl; }
            if (messages.size() == 3) {
                //From OParent
                oWebsiteConnection->sendData(messages[0] + "." + messages[1]);
                websiteParentConnection->sendData(helper->decodeHex(messages[2]));

            } else if (messages.size() > 0) {
                //From OChild
                std::string toWebsite = helper->decodeHex(messages[0]);
                websiteChildConnection->sendData(toWebsite);
            }

        }
    }

 public:
    RWebsite(int parentPort, int childPort) {
        iWebsiteConnection      = new Connection(LOCALHOST, I_INTERNAL_PORT);
        oWebsiteConnection      = new Connection(LOCALHOST, O_INTERNAL_PORT);
        websiteParentConnection = new Connection(LOCALHOST, parentPort);
        websiteChildConnection  = new Connection(LOCALHOST, childPort);
        helper                  = new InterfaceHelper();
    }

    void run() {
        Poco::RunnableAdapter<RWebsite> iWebsiteFuncAdapt(*this, &RWebsite::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);

        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        Poco::Thread websiteParentConnectionThread;
        websiteParentConnectionThread.start(*websiteParentConnection);

        Poco::Thread websiteChildConnectionThread;
        websiteChildConnectionThread.start(*websiteChildConnection);

        oWebsiteConnection->waitForEstablishment();
        websiteParentConnection->waitForEstablishment();

        /* Forwards policy data from Website directly to OWebsite in order
         * to be sent to the Parent.
         */
        std::string incoming;
        while (true) {
            websiteParentConnection->waitForReceivedData();
            incoming = websiteParentConnection->getData();

            std::cout << "Received from Website: " << incoming << std::endl;

            oWebsiteConnection->sendData(helper->encodeHex(incoming));
        }

    }
};

int main(int argc, char **argv) {
    RWebsite rWebsite(std::stoi(argv[1]), std::stoi(argv[2]));
    rWebsite.run();

    return 0;
}


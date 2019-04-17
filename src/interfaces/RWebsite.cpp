#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

class RWebsite {
 private:
    Connection      *iWebsiteConnection, *oWebsiteConnection, *websiteParentConnection, *websiteChildConnection;
    InterfaceHelper *helper;

    // ADD COMMENTS HERE
    void iWebsiteConnectionHandler() {
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        iWebsiteConnection->waitForEstablishment();
        oWebsiteConnection->waitForEstablishment();
        websiteChildConnection->waitForEstablishment();
        websiteParentConnection->waitForEstablishment();

        std::string incoming;
        while (true) {
            iWebsiteConnection->waitForReceivedData();
            incoming = iWebsiteConnection->getData();
            std::cout << "Received from iWebsite: " << incoming << std::endl;
            std::vector<std::string> messages = helper->splitString(incoming, '.');

            if (messages.size() > 2) {
                //From OChild
                std::string toOWebsite = messages[0] + '.' + messages[1];
                std::string toWebsite =  helper->decodeHex(messages[2]);

                oWebsiteConnection->sendData(toOWebsite);
                websiteChildConnection->sendData(toWebsite);

            } else if (messages.size() > 1) {
                //From OParent
                oWebsiteConnection->sendData(incoming);
            }

            if (DEBUG) {
                if (incoming == "consent") {
                    websiteParentConnection->sendData(incoming);
                } else {
                    websiteChildConnection->sendData(incoming);
                }
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

        //ADD COMMNETS HERE
        // Main thread continues waiting for data from Website
        std::string incoming;
        while (true) { // Get policy from Website, forward to Parent via OWebsite
            websiteParentConnection->waitForReceivedData();
            incoming = websiteParentConnection->getData();

            std::cout << "Received from Website: " << incoming << std::endl;

            oWebsiteConnection->sendData(helper->encodeHex(incoming));
            if (DEBUG) { std::cout << "Sending it to OWebsite" << std::endl; oWebsiteConnection->sendData(incoming); }
        }

    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogRW.txt", "a", stdout);
    RWebsite rWebsite(std::stoi(argv[1]), std::stoi(argv[2]));
    rWebsite.run();

    return 0;
}


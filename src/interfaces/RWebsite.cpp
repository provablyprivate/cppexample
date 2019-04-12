#include "./Constants.h"
#include "../Connection.h"
#include <stdlib.h>

class RWebsite {
private:
    Connection *iWebsiteConnection;
    Connection *oWebsiteConnection;
    Connection *websiteParentConnection;
    Connection *websiteChildConnection;

    void iWebsiteConnectionHandler() {
        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);

        std::string s;
        while (true) {
            iWebsiteConnection->waitForReceivedData();
            s = iWebsiteConnection->getData();
            std::cout << "Received from iWebsite: " << s << std::endl;
            if (DEBUG) { if (s == "consent") websiteParentConnection->sendData(s); else websiteChildConnection->sendData(s); }
            // check if it's coming from Parent (consent) or Child (pdata)
            // forward pdata to website (over websiteChildConnection),
            // and consent to website (over websiteParentConnection) and also to owebsite (over oWebsiteconnection)
        }
    }




public:
    RWebsite(int parentPort, int childPort) {
        iWebsiteConnection = new Connection(LOCALHOST, I_INTERNAL_PORT);
        oWebsiteConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        websiteParentConnection = new Connection(LOCALHOST, parentPort);
        sleep(1);
        websiteChildConnection = new Connection(LOCALHOST, childPort);
    }

    void run() {
        Poco::RunnableAdapter<RWebsite> iWebsiteFuncAdapt(*this, &RWebsite::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);

        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);


        websiteParentConnection->waitForEstablishment();
        Poco::Thread websiteParentConnectionThread;
        websiteParentConnectionThread.start(*websiteParentConnection);

        websiteChildConnection->waitForEstablishment();
        Poco::Thread websiteChildConnectionThread;
        websiteChildConnectionThread.start(*websiteChildConnection);

        // Main thread continues waiting for data from Website
        std::string s;
        while (true) { // Get policy from Website, forward to Parent via OWebsite
            websiteParentConnection->waitForReceivedData();
            s = websiteParentConnection->getData();
            std::cout << "Received from Website: " << s << ", sending it to OWebsite" << std::endl;
            oWebsiteConnection->sendData(s);
        }

    }
};


int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogRW.txt", "a", stdout);
    RWebsite rWebsite(std::stoi(argv[1]), std::stoi(argv[2]));
    rWebsite.run();

    return 0;
}


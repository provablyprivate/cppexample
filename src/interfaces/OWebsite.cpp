#include "./Constants.h"
#include "../Connection.h"


class OWebsite {
private:
    Connection *rWebsiteConnection;
    Connection *oChildConnection;
    Connection *iParentConnection;

public:
    OWebsite() {
        rWebsiteConnection = new Connection(O_INTERNAL_PORT);
        oChildConnection = new Connection(O_EXTERNAL_PORT_1);
        iParentConnection = new Connection(O_EXTERNAL_PORT_2);
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
            // check if it's the consent json, and if so forward to OChild (over oCHildConnection)
            //if it's the policy, encrypt for parent and forward to IParent (over iParentConnection)
            if (DEBUG) { std::cout << "Sending it to IParent" << std::endl; iParentConnection->sendData(s); }
        }
    }


};


int main() {
    //if (DEBUG) freopen("./errorlogOW.txt", "a", stdout);
    OWebsite oWebsite;
    oWebsite.run();
}

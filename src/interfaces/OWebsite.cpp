#include "./Constants.h"
#include "../Connection.h"


class OWebsite {
private:
    Connection *rWebsiteConnection;
    Connection *oChildConnection;
    Connection *iParentConnection;

public:
    OWebsite() {
        rWebsiteConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        oChildConnection = new Connection(O_EXTERNAL_PORT_1);
        iParentConnection = new Connection(O_EXTERNAL_PORT_2);
    }

    void run() {
        rWebsiteConnection->waitForEstablishment();
        Poco::Thread rWebsiteConnectionThread;
        rWebsiteConnectionThread.start(*rWebsiteConnection);

        oChildConnection->waitForEstablishment();
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        iParentConnection->waitForEstablishment();
        Poco::Thread iParentConnectionThread;
        iParentConnectionThread.start(*iParentConnection);

        std::string s;
        while (true) {
            rWebsiteConnection->waitForReceivedData();
            s = rWebsiteConnection->getData();
            // check if it's the consent json, and if so forward to OChild (over oCHildConnection)
            //if it's the policy, encrypt for parent and forward to IParent (over iParentConnection)
        }
    }


};


int main() {
    OWebsite oWebsite;
    oWebsite.run();
}

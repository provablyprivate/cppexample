#include "./Constants.h"
#include "../Connection.h"


class IWebsite {
private:
    Connection *rWebsiteConnection;
    Connection *oChildConnection;
    Connection *oParentConnection;

    void oChildConnectionHandler() {
        oChildConnection->waitForEstablishment();
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        std::string s; // pdata goes here
        while (true) {
            oChildConnection->waitForReceivedData();
            s = oChildConnection->getData();
            //decrypt etc, pass to RWebsite
        }
    }

public:
    IWebsite() {
        rWebsiteConnection = new Connection(LOCALHOST, I_INTERNAL_PORT);
        oChildConnection = new Connection(I_EXTERNAL_PORT_1);
        oParentConnection = new Connection(I_EXTERNAL_PORT_2);
    }

    void run() {
        rWebsiteConnection->waitForEstablishment();
        Poco::Thread rWebsiteConnectionThread;
        rWebsiteConnectionThread.start(*rWebsiteConnection);

        Poco::RunnableAdapter<IWebsite> oChildFuncAdapt(*this, &IWebsite::oChildConnectionHandler);
        Poco::Thread oChildConnectionHandlerThread;
        oChildConnectionHandlerThread.start(oChildFuncAdapt);

        oParentConnection->waitForEstablishment();
        Poco::Thread oParentConnectionThread;
        oParentConnectionThread.start(*oParentConnection);

        std::string s; // consent goes here
        while (true) {
            oParentConnection->waitForReceivedData();
            s = oParentConnection->getData();
            // decrypt, verify signature etc, pass to RWebsite
        }
    }


};

int main() {
    IWebsite iWebsite;
    iWebsite.run();
}

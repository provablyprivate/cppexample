#include "Constants.h"
#include "Connection.h"


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
            if (DEBUG) { std::cout << "Received from OChild: " << s << ", sending it to RWebsite" << std::endl; rWebsiteConnection->sendData(s); }
        }
    }
    
public:
    IWebsite() {
        rWebsiteConnection = new Connection(I_INTERNAL_PORT);
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
            if (DEBUG) { std::cout << "Received from OParent: " << s << ", sending it to RWebsite" << std::endl; rWebsiteConnection->sendData(s); }
        }
    }

    
};
    
int main() {
    //if (DEBUG) freopen("./errorlogIW.txt", "a", stdout);
    IWebsite iWebsite;
    iWebsite.run();
}

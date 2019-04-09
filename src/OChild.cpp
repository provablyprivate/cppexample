#include "Constants.h"
#include "Connection.h"


class OChild {

private:
    Connection *rChildConnection;
    Connection *iWebsiteConnection;
    Connection *oWebsiteConnection;
    
    void rChildConnectionHandler() {
        rChildConnection->waitForEstablishment();
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);
        
        std::string s;
        while (true) {
            rChildConnection->waitForReceivedData();
            s = rChildConnection->getData();
            // assemble term
            std::cout << "Received from RChild: " << s << std::endl;
            iWebsiteConnection->sendData(s);
        }
    }
    
    /*void iWebsiteConnectionHandler() {
        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);
        
        while (true) {
            
        }
    }*/
    
    void oWebsiteConnectionHandler() {
        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);
        
        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;
        }
    }
    
public:
    OChild(std::string websiteIP) {
        rChildConnection = new Connection(O_INTERNAL_PORT);
        iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_1);
        oWebsiteConnection = new Connection(O_EXTERNAL_PORT_1);
    }
    
    void run() {

        Poco::RunnableAdapter<OChild> rChildFuncAdapt(*this, &OChild::rChildConnectionHandler);
        Poco::Thread rChildConnectionHandlerThread;
        rChildConnectionHandlerThread.start(rChildFuncAdapt);

        /*Poco::RunnableAdapter<OChild> iWebsiteFuncAdapt(*this, &OChild::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);*/

        Poco::RunnableAdapter<OChild> oWebsiteFuncAdapt(*this, &OChild::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);
        
        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);,
    }
    
};

int main(int argc, char **argv) {
    OChild oChild(argv[2]);
    oChild.run();
    
    return 0;
}


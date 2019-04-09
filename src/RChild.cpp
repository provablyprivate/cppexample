#include "Constants.h"
#include "Connection.h"


class RChild {

private:
    Connection *childConnection;
    Connection *oChildConnection;
    
    void relayData() {
        std::string s;
        while (true) {
            childConnection->waitForReceivedData();
            s = childConnection->getData();
            oChildConnection->sendData(s);
        }
        
    }
    
public:
    RChild(int websitePortUsedByChild) {
        childConnection = new Connection(websitePortUsedByChild);
        oChildConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
    }
    
    void run() {
        
        oChildConnection->waitForEstablishment();
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);
        
        childConnection->waitForEstablishment();
        Poco::Thread childConnectionThread;
        childConnectionThread.start(*childConnection);
        
        relayData();
    }

};

int main(int argc, char **argv) {
    RChild rChild(std::stoi(argv[2]));
    rChild.run();
    
    return 0;
}

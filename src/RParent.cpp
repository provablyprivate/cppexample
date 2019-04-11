#include "Constants.h"
#include "Connection.h"
#include <stdlib.h>

class RParent {
private:
    Connection *iParentConnection;
    Connection *oParentConnection;
    Connection *parentConnection;
    
    void iParentConnectionHandler() {
        iParentConnection->waitForEstablishment();
        Poco::Thread iParentConnectionThread;
        iParentConnectionThread.start(*iParentConnection);
        
        std::string s;
        while (true) {
            iParentConnection->waitForReceivedData();
            s = iParentConnection->getData();
            std::cout << "Received from IParent: " << s << std::endl;
            // pass it along to parent (and oparent?)
            if (DEBUG) { std::cout << "Sending it to Parent" << std::endl; parentConnection->sendData(s); }
        }
    }
    
public:
    RParent(int webSitePortUsedByParent) {
        iParentConnection = new Connection(LOCALHOST, I_INTERNAL_PORT); sleep(1);
        oParentConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        parentConnection = new Connection(webSitePortUsedByParent);
    }
    
    void run() {
        Poco::RunnableAdapter<RParent> iParentFuncAdapt(*this, &RParent::iParentConnectionHandler);
        Poco::Thread iParentConnectionHandlerThread;
        iParentConnectionHandlerThread.start(iParentFuncAdapt);
        
        oParentConnection->waitForEstablishment();
        Poco::Thread oParentConnectionThread;
        oParentConnectionThread.start(*oParentConnection);
        
        parentConnection->waitForEstablishment();
        Poco::Thread parentConnectionThread;
        parentConnectionThread.start(*parentConnection);
        
        std::string s;
        while (true) {
            parentConnection->waitForReceivedData();
            s = parentConnection->getData();
            std::cout << "Received from Parent: " << s << std::endl;
            if (DEBUG) { std::cout << "Sending it to OParent" << std::endl; oParentConnection->sendData(s); }
        }
    }
    
};
    
int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogRP.txt", "a", stdout);
    RParent rParent(std::stoi(argv[1]));
    rParent.run();
    
    return 0;
}

#include "Constants.h"
#include "Connection.h"


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
            // skicka vidare till parent eller oparent
        }
    }
    
public:
    RParent(int webSitePortUsedByParent) {
        iParentConnection = new Connection(LOCALHOST, I_INTERNAL_PORT);
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
            oParentConnection->sendData(s);
        }
    }
    
};
    
int main(int argc, char **argv) {
    RParent rParent(std::stoi(argv[1]));
    rParent.run();
    
    return 0;
}

#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"
#include <stdlib.h>
#include <sstream>
#include "Poco/HexBinaryDecoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/StreamCopier.h"

class RParent {
private:
    Connection *iParentConnection;
    Connection *oParentConnection;
    Connection *parentConnection;
    InterfaceHelper * helper;

    void iParentConnectionHandler() {
        iParentConnection->waitForEstablishment();
        Poco::Thread iParentConnectionThread;
        iParentConnectionThread.start(*iParentConnection);

        std::string s;
        while (true) {
            iParentConnection->waitForReceivedData();
            s = iParentConnection->getData();
            std::cout << "Received from IParent: " << s << std::endl;

            std::vector<std::string> messages = helper->splitString(s, '.');
            //since we know that the message is going to be hex.hex.hex we can do following

            std::string toOParent = messages[0] + '.' + messages[1];
            std::string toParent = helper->decodeHex(messages[2]);

            oParentConnection->sendData(toOParent);
            parentConnection->sendData(toParent);

            // pass it along to parent (and oparent?)
            if (DEBUG) { std::cout << "Sending it to Parent" << std::endl; parentConnection->sendData(s); }
        }
    }

public:
    RParent(int webSitePortUsedByParent) {
        iParentConnection = new Connection(LOCALHOST, I_INTERNAL_PORT); sleep(1);
        oParentConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        parentConnection  = new Connection(webSitePortUsedByParent);
        helper            = new InterfaceHelper();
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

            oParentConnection->sendData(helper->encodeHex(s));
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

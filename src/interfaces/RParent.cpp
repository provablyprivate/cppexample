#include "./Constants.h"
#include "./InterfaceHelper.cpp"
#include "../Connection.h"

class RParent {
 private:
    Connection      *iParentConnection, *oParentConnection, *parentConnection;
    InterfaceHelper *helper;

    /* Decodes and decrypts the policy data to parent.
     * Encodes the decrypted data back to hex. The original
     * message will be sent to OParent, whereas the decrypted
     * messages gets sent to Parent.
     */
    void iParentConnectionHandler() {
        Poco::Thread iParentConnectionThread;
        iParentConnectionThread.start(*iParentConnection);

        iParentConnection->waitForEstablishment();
        oParentConnection->waitForEstablishment();
        parentConnection->waitForEstablishment();
        std::string incoming;
        while (true) {
            iParentConnection->waitForReceivedData();
            incoming = iParentConnection->getData();
            std::cout << "Received from IParent: " << incoming << std::endl;
            std::vector<std::string> messages = helper->splitString(incoming, '.');

            std::string toOParent = messages[0] + '.' + messages[1];
            std::string toParent = helper->decodeHex(messages[2]);

            oParentConnection->sendData(toOParent);
            parentConnection->sendData(toParent);

            if (DEBUG) { std::cout << "Sending it to Parent" << std::endl; parentConnection->sendData(incoming); }
        }
    }

 public:
    RParent(int webSitePortUsedByParent) {
        iParentConnection = new Connection(LOCALHOST, I_INTERNAL_PORT);
        oParentConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        parentConnection  = new Connection(webSitePortUsedByParent);
        helper            = new InterfaceHelper();
    }

    void run() {
        Poco::RunnableAdapter<RParent> iParentFuncAdapt(*this, &RParent::iParentConnectionHandler);
        Poco::Thread iParentConnectionHandlerThread;
        iParentConnectionHandlerThread.start(iParentFuncAdapt);

        Poco::Thread oParentConnectionThread;
        oParentConnectionThread.start(*oParentConnection);

        Poco::Thread parentConnectionThread;
        parentConnectionThread.start(*parentConnection);

        oParentConnection->waitForEstablishment();
        parentConnection->waitForEstablishment();

        /* Relays all messages from Parents directly to OParent. */
        std::string incoming;
        while (true) {
            parentConnection->waitForReceivedData();
            incoming = parentConnection->getData();
            std::cout << "Received from Parent: " << incoming << std::endl;

            oParentConnection->sendData(helper->encodeHex(incoming));
            if (DEBUG) { std::cout << "Sending it to OParent" << std::endl; oParentConnection->sendData(incoming); }
        }
    }
};

int main(int argc, char **argv) {
    //if (DEBUG) freopen("./errorlogRP.txt", "a", stdout);
    RParent rParent(std::stoi(argv[1]));
    rParent.run();

    return 0;
}

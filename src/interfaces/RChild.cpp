#include "./Constants.h"
#include "../Connection.h"


class RChild {

private:
    Connection *childConnection;
    Connection *oChildConnection;

    void relayData() {
        oChildConnection->waitForEstablishment();
        childConnection->waitForEstablishment();
        
        std::string s;
        while (true) {
            childConnection->waitForReceivedData();
            s = childConnection->getData();
            std::cout << "Received " << s << " from Child, sending it to OChild" << std::endl;
            oChildConnection->sendData(s);
        }

    }

public:
    RChild(int websitePortUsedByChild) {
        oChildConnection = new Connection(LOCALHOST, O_INTERNAL_PORT);
        childConnection = new Connection(websitePortUsedByChild);
    }

    void run() {
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        Poco::Thread childConnectionThread;
        childConnectionThread.start(*childConnection);

        relayData();
    }

};

int main(int argc, char **argv) {
    RChild rChild(std::stoi(argv[1]));
    rChild.run();

    return 0;
}

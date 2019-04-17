#include "./interfaces/Constants.h"
#include "./Connection.h"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

class Website {
 private:
    Connection *parentConnection;
    Poco::Thread parentConnectionThread;
    Connection *childConnection;
    Poco::Thread childConnectionThread;

    std::string policy;
    std::string receivedParentData;
    std::string receivedChildData;

    void createPolicy() {
        policy = "policy";
    }

    void readIncomingChildData() {
        childConnection->waitForEstablishment();
        while (true) {
            childConnection->waitForReceivedData();
            receivedChildData = childConnection->getData();
            std::cout << "Received from Child: " << receivedChildData << std::endl;
        }
    }

    void readIncomingParentData() {
        parentConnection->waitForEstablishment();
        while (true) {
            parentConnection->waitForReceivedData();
            receivedParentData = parentConnection->getData();
            std::cout << "Received from Parent: " << receivedParentData << std::endl;
        }
    }

 public:
    Website(int parentPort, int childPort) {
        parentConnection = new Connection(parentPort);
        childConnection = new Connection(childPort);
    }

    void run() {
        createPolicy();

        parentConnectionThread.start(*parentConnection);
        Poco::RunnableAdapter<Website> readerFuncAdaptParent(*this, &Website::readIncomingParentData);
        Poco::Thread parentReaderThread;
        parentReaderThread.start(readerFuncAdaptParent);

        childConnectionThread.start(*childConnection);
        Poco::RunnableAdapter<Website> readerFuncAdaptChild(*this, &Website::readIncomingChildData);
        Poco::Thread childReaderThread;
        childReaderThread.start(readerFuncAdaptChild);


        srand(time(NULL) + 123);
        parentConnection->waitForEstablishment();
        while (true) {
            sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            // std::cout << "Sending to Parent: " << policy << std::endl;
            // parentConnection->sendData(policy);
        }
    }
};

int main(int argc, char **argv) {
    try {
        Website website(std::stoi(argv[1]), std::stoi(argv[2]));
        website.run();
    }
    catch (Poco::IOException e) {
        std::cout << e.displayText() << e.what();
    }
    return 0;
}

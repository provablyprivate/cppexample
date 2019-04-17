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

    bool automatic;

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
    Website(int parentPort, int childPort, bool autoSend) {
        parentConnection = new Connection(parentPort);
        childConnection = new Connection(childPort);
        automatic = autoSend;
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
        
        parentConnection->waitForEstablishment();
        
        srand(time(NULL) + 123);
        while (true) {
        
            if (automatic) {
                sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            }
            else {
                std::cout << "\nPress enter to send POLICY" << std::endl;
                getchar();
            }
            
            std::cout << "Sending to Parent: " << policy << std::endl;
            parentConnection->sendData(policy);
        }
        
    }
};

int main(int argc, char **argv) {
    Website website(std::stoi(argv[1]), std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    website.run();
    
    return 0;
}

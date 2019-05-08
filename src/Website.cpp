#include "./interfaces/Constants.h"
#include "./Connection.h"
#include "./JSONhandler.cpp"
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

    JSONHandler *policyJSON;
    std::string receivedParentData;
    std::string receivedChildData;

    bool automatic;

    void createPolicy() {
        policyJSON = new JSONHandler();
        policyJSON->put("Type", "POLICY");
        policyJSON->put("Value", "This is some policy bla bla");
    }
    

    void readIncomingChildData() {
        childConnection->waitForEstablishment();
        while (true) {
            childConnection->waitForReceivedData();
            receivedChildData = childConnection->getData();
            JSONHandler *pdataJSON = new JSONHandler(receivedChildData);
            std::cout << "\nReceived a piece of data from Child: " << std::endl;
            pdataJSON->printAsTerm();
        }
    }

    void readIncomingParentData() {
        parentConnection->waitForEstablishment();
        while (true) {
            parentConnection->waitForReceivedData();
            receivedParentData = parentConnection->getData();
            JSONHandler *consentJSON = new JSONHandler(receivedParentData);
            std::cout << "\nReceived a piece of data from Parent:" << std::endl;
            consentJSON->printAsTerm();
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
            
            std::cout << "Sending a piece of data to Parent: " << std::endl;
            policyJSON->printAsTerm();
            parentConnection->sendData(policyJSON->toString());
        }
        
    }
};

int main(int argc, char **argv) {
    Website website(std::stoi(argv[1]), std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    website.run();
    
    return 0;
}

#include "./interfaces/Constants.h"
#include "./Connection.h"
#include "./JSONhandler.cpp"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

class Parent {
 private:
    Connection *websiteConnection;
    JSONHandler *consentJSON;
    Poco::Thread readerThread;
    std::string receivedData;
    bool automatic;
    
    void createConsent() {
        consentJSON = new JSONHandler();
        consentJSON->put("Type", "CONSENT");
        consentJSON->put("Value", "");
    }

    void readIncomingData() {
        websiteConnection->waitForEstablishment();
        while (true) {
            websiteConnection->waitForReceivedData();
            receivedData = websiteConnection->getData();
            JSONHandler *policyJSON = new JSONHandler(receivedData);
            std::cout << "\nReceived a piece of data from Website: " << std::endl;
            printAsTerm(policyJSON);
        }
    }
    
    void printAsTerm(JSONHandler *json) {
        std::cout << " Type: " << (std::string) json->get("Type") << "\n Value: " << (std::string) json->get("Value") << std::endl;
    }

 public:
    Parent(std::string websiteIP, int websitePort, bool autoSend) {
        websiteConnection = new Connection(websiteIP, websitePort);
        automatic = autoSend;
    }

    void run() {
        createConsent();
        Poco::Thread connectionThread;
        connectionThread.start(*websiteConnection);

        Poco::RunnableAdapter<Parent> readerFuncAdapt(*this, &Parent::readIncomingData);
        readerThread.start(readerFuncAdapt);

        websiteConnection->waitForEstablishment();
        
        srand(time(NULL) + 456);
        while (true) {
        
            if (automatic) {
                sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            }
            else {
                std::cout << "\nPress enter to send CONSENT" << std::endl;
                getchar();
            }
            
            std::cout << "Sending a piece of data to Website: " << std::endl;
            printAsTerm(consentJSON);
            websiteConnection->sendData(consentJSON->toString());
        }
    }
};

int main(int argc, char **argv) {
    Parent parent(argv[1], std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    parent.run();

    return 0;
}

#include "./interfaces/Constants.h"
#include "./Connection.h"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

class Parent {
 private:
    Connection *websiteConnection;
    std::string consent;
    Poco::Thread readerThread;
    std::string receivedData;
    bool automatic;
    void createConsent() {
        consent = "consent";
    }

    void readIncomingData() {
        websiteConnection->waitForEstablishment();
        while (true) {
            websiteConnection->waitForReceivedData();
            receivedData = websiteConnection->getData();
            std::cout << "Received from Website: " << receivedData << std::endl;
        }
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
            
            std::cout << "Sending to Website: " << consent << std::endl;
            websiteConnection->sendData(consent);
        }
    }
};

int main(int argc, char **argv) {
    Parent parent(argv[1], std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    parent.run();

    return 0;
}

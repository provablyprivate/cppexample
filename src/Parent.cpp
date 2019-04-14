#include "./interfaces/Constants.h"
#include "./Connection.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

class Parent {

private:
    Connection *websiteConnection;
    std::string consent;
    Poco::Thread readerThread;
    std::string receivedData;
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
    Parent(std::string websiteIP, int websitePort) {
        websiteConnection = new Connection(websiteIP, websitePort);
    }


    void run() {
        createConsent();
        Poco::Thread connectionThread;
        connectionThread.start(*websiteConnection);

        Poco::RunnableAdapter<Parent> readerFuncAdapt(*this, &Parent::readIncomingData);
        readerThread.start(readerFuncAdapt);

        srand(time(NULL) + 456);
        websiteConnection->waitForEstablishment();
        while (true) {
            sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            //std::cout << "Sending to Website: " << consent << std::endl;
            //websiteConnection->sendData(consent);
        }

    }


};

int main(int argc, char **argv) {
    Parent parent(argv[1], std::stoi(argv[2]));
    parent.run();

    return 0;
}

#include "./interfaces/Constants.h"
#include "./Connection.h"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

class Child {
 private:
    Connection *websiteConnection;

    std::string privateData;

    bool automatic;

    void createPrivateData() {
        privateData = "some private data";
    }

 public:
    Child(std::string websiteIP, int websitePort, bool autoSend) {
        websiteConnection = new Connection(websiteIP, websitePort);
        automatic = autoSend;
    }

    void run() {
        createPrivateData();
        Poco::Thread connectionThread;
        connectionThread.start(*websiteConnection);

        websiteConnection->waitForEstablishment();
        
        srand(time(NULL) + 789);
        while (true) {
        
            if (automatic) {
                sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            }
            else {
                std::cout << "\nPress enter to send PDATA" << std::endl;
                getchar();
            }
            
            std::cout << "Sending to Website: " << privateData << std::endl;
            websiteConnection->sendData(privateData);
        }

    }
};

int main(int argc, char **argv) {
    Child child(argv[1], std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    child.run();

    return 0;
}

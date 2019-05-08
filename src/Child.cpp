#include "./interfaces/Constants.h"
#include "./Connection.h"
#include "./JSONhandler.cpp"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

class Child {
 private:
    Connection *websiteConnection;
    
    JSONHandler *pdataJSON;

    bool automatic;

    void createPrivateData() {
        pdataJSON = new JSONHandler();
        pdataJSON->put("Type", "PDATA");
        pdataJSON->put("Value", "This is some private data");
    }
    
    void printAsTerm(JSONHandler *json) {
        std::cout << " Type: " << (std::string) json->get("Type") << "\n Value: " << (std::string) json->get("Value") << std::endl;
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
            
            std::cout << "Sending a piece of data to Website:" << std::endl;
            printAsTerm(pdataJSON);
            websiteConnection->sendData(pdataJSON->toString());
        }

    }
};

int main(int argc, char **argv) {
    Child child(argv[1], std::stoi(argv[2]), (strcmp(argv[3], "True") == 0) ? true : false);
    child.run();

    return 0;
}

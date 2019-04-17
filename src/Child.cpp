#include "./interfaces/Constants.h"
#include "./Connection.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

class Child {
 private:
    Connection *websiteConnection;

    std::string privateData;

    void createPrivateData() {
        privateData = "private data";
    }

 public:
    Child(std::string websiteIP, int websitePort) {
        websiteConnection = new Connection(websiteIP, websitePort);
    }

    void run() {
        createPrivateData();
        Poco::Thread connectionThread;
        connectionThread.start(*websiteConnection);

        srand(time(NULL) + 789);
        websiteConnection->waitForEstablishment();
        while (true) {
            sleep(rand() % 20 + 1); // Sleep between 1 and 20 seconds
            //std::cout << "Sending to Website: " << privateData << std::endl;
            //websiteConnection->sendData(privateData);
        }
    }
};

int main(int argc, char **argv) {
    Child child(argv[1], std::stoi(argv[2]));
    child.run();

    return 0;
}

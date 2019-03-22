/*
 * Client code for an example chat application using Connection and ConnectionInterface
 */
#include "mininetexample.h"
#include "ConnectionInterface.h"
#include "Connection.h"
#include <csignal>

void sigHandler(int signal) {
    if (signal == SIGINT) {
        exit(0);
    }
}

void readKeyboardInput(ConnectionInterface iface) {
    std::string s;
    while (true) {
        std::getline(std::cin, s);
        iface.sendData(s); // Data is sent with this call
    }
}

int main() {
    signal(SIGINT, sigHandler);
    
    ConnectionInterface iface;
    Connection connection(IP, PORT, &iface);
    iface.connectionEstablished.wait();
    Poco::Thread connectionThread;
    connectionThread.start(connection);
    
    Poco::Thread inputThread;
    inputThread.start((void (*)(void*)) &readKeyboardInput, &iface);
    
    std::string s;
    while (true) {
        iface.dataReceived.wait(); // Wait for Connection to signal that data has arrived
        s = iface.getData(); // Retrieve the data
        std::cout << s << std::endl;
        if (iface.connectionClosed.tryWait(1)) {
            inputThread.~Thread();
            kill(getpid(), SIGINT);
            return 0;
        }
    }
    
    return 0;
}

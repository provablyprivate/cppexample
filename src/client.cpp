#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"

#include <string>
#include <iostream>
#include "mininetexample.h"

int main(int argc, char** argv) {
    Poco::Net::SocketAddress address(argv[1], PORT);
    Poco::Net::StreamSocket streamSocket(address);
    Poco::Net::SocketStream socketStream(streamSocket);
    std::string s;
    std::cout << "Connection to server established. Exit with bye" << std::endl;

    while (true) {
        std::getline(socketStream, s);
        std::cout << "Server responded with " << s << std::endl;
        std::cout << ">> ";
        std::getline(std::cin, s);
        std::cout << "Sending " << s << " to server" << std::endl;
        socketStream << s << std::endl;
        if (s == "bye") {
            return 0;
        }


    }

    int slutanu = argc;
    slutanu = 0;
    return slutanu;
}

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"

#include "mininetexample.h"

#include <algorithm>
#include <string>
#include <iostream>


int main() {
    Poco::Net::ServerSocket listener;
    listener = Poco::Net::ServerSocket(PORT, 64);
    Poco::Net::StreamSocket streamSocket = listener.acceptConnection();
    Poco::Net::SocketStream socketStream(streamSocket);
    std::cout << "Connection to client established" << std::endl;

    std::string s;

    while (true) {
        //socketStream << "Send me something and get it back in uppercase (exit with bye)" << std::endl;
        std::cout << ">> ";
        std::getline(std::cin, s);
        std::cout << "Sending " << s << " to client" << std::endl;
        socketStream << s << std::endl;
        std::getline(socketStream, s);
        if (s == "bye") {
            socketStream << "BYE" << std::endl;
            std::cout << "Client exit" << std::endl;
            listener.~ServerSocket();
            return 0;
        }
        std::cout << "Received from client: " << s << std::endl;
        // Turn s into upper case
        //std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> unsigned char { return std::toupper(c); });
    }
}

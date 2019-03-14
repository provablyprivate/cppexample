#include "Poco/Event.h"

/*
 * Class that handles communication and data passing between Connection objects and their users
 */
class ConnectionInterface {

public:
    std::string arrivedData; // Data received from the socket that is to be passed to the client
    std::string unsentData; // Data from the user that is to be sent through the socket
    
    // Connection sets these to notify the user
    Poco::Event connectionEstablished = Poco::Event(true);
    Poco::Event dataReceived = Poco::Event(true);
    
    // This is set when there is new data from the user to send
    Poco::Event dataToSend = Poco::Event(true);
    
    // This can be used by either users or Connections
    Poco::Event connectionClosed = Poco::Event(false);
    
    // Send data through the socket
    void sendData(std::string s) {
        this->unsentData = s;
        this->dataToSend.set();
    }
    
    // Receive data from the socket. Callers should wait for the dataReceived event before calling
    std::string getData() {
        return arrivedData;
    }

}; //TODO public setters/getters and non-public instance variables

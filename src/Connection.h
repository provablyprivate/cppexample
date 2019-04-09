#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/ThreadTarget.h"
#include "Poco/Event.h"

/*
 * Class that models connections to remote hosts. Used with a ConnectionInterface to hide low-level details
 * If instantiated with socket and ConnectionInterface, it waits for incoming connections
 * If instantiated with ip, socket and ConnectionInterface, it attempts to connect to the socket at the ip
 */
class Connection: public Poco::Runnable {

private:
    std::string arrivedData;
    std::string unsentData;
    
    Poco::Event connectionEstablished = Poco::Event(true);
    Poco::Event dataReceived = Poco::Event(true);
    Poco::Event dataToSend = Poco::Event(true);
    Poco::Event connectionClosed = Poco::Event(false);

    Poco::Net::SocketAddress address; // Used by the part that initiates the connection (the "client")
    Poco::Net::ServerSocket listener; // Used by the part that accepts the connection attempt (the "server")
    // Used by both:
    Poco::Net::StreamSocket streamSocket;
    Poco::Net::SocketStream *socketStream;
    Poco::Thread readerThread;
    Poco::Thread writerThread;
    

    // Creates the socket used to pass and receive messages, and signals the user that the connection is up
    void setUp() {
        socketStream = new Poco::Net::SocketStream(streamSocket);
        connectionEstablished.set();
    }
    
    // Does a blocking read on the socket. Upon success, transfers data to connection interface and notifies user
    void readSocket() {
        std::string s;
        while (s != connectionTerminator) { 
            std::getline(*socketStream, s);
            arrivedData = s;
            dataReceived.set();
        }
        connectionClosed.set();
    }
    
    // Waits for data from the user to become available, then sends it through the socket
    void writeSocket() {
        while (unsentData != connectionTerminator) {
            dataToSend.wait();
            *socketStream << this->unsentData << std::endl;
        }
        connectionClosed.set();
    }
    
public:
    // Tears down the connection
    ~Connection() {
        address.~SocketAddress();
        listener.~ServerSocket();
        streamSocket.~StreamSocket();
        readerThread.~Thread();
        writerThread.~Thread();
        delete socketStream;
    }
    
    // Called if this instance acts as server
    Connection(int port) {
        listener = Poco::Net::ServerSocket(port, 64);
        streamSocket = listener.acceptConnection();
        setUp();
    }
    
    // Called if this instance acts as client
    Connection(std::string ip, int port) {
        address = Poco::Net::SocketAddress(ip, port);
        streamSocket = Poco::Net::StreamSocket(address);
        setUp();
    }
    
    void waitForEstablishment() {
        connectionEstablished.wait();
    }
    
    void waitForReceivedData() {
        dataReceived.wait();
    }
    
    void sendData(std::string s) {
        unsentData = s;
        dataToSend.set();
    }
    
    std::string getData() {
        return arrivedData;
    }
    
    // Called when a thread is started with this Connection object (after connectionEstablished has been signalled)
    // Creates two threads that, respectively reads from and writes to the socket
    virtual void run() {
        Poco::RunnableAdapter<Connection> readerFuncAdapt(*this, &Connection::readSocket);
        Poco::RunnableAdapter<Connection> writerFuncAdapt(*this, &Connection::writeSocket);
        readerThread.start(readerFuncAdapt); 
        writerThread.start(writerFuncAdapt);
        
        // Wait for either of the threads to close the connection (one always will)
        connectionClosed.wait();
    }

};

#include <iostream>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "./InterfaceHelper.cpp"
#include "../Crypt.cpp"
#include "../JSONhandler.cpp"

InterfaceHelper *helper = new InterfaceHelper();


void verify(std::string sender, std::string receiver, std::string message) {
    
    std::vector<std::string> messageParts = helper->splitString(message, '.');
    std::string jsonStr;
        
    // Non-extended architecture
    if (messageParts.size() == 1) {
        jsonStr = messageParts[0];
    }
    
    // Extended architecture
    if (messageParts.size() == 2) {
        jsonStr = helper->decodeHex(messageParts[0]);
        std::string signature = helper->decodeHex(messageParts[1]);
        //TODO: check signatures etc
    }
    
    JSONHandler *json = new JSONHandler(jsonStr);
    
    std::cout << "\n" << sender << " -> " << receiver << ":\n" << "Type: " << (json->get("Type")).toString() << "\nComplete JSON: " << json->toString() << std::endl << std::endl;
}

int main(int argc, char **argv) {
    std::string ifaceID = argv[1];
    
    // Store names for the different hosts
    std::map<std::string, std::string> hostIP;
    hostIP[argv[2]] = "Website";
    hostIP[argv[3]] = "Parent";
    hostIP[argv[4]] = "Child";
    
    int p[2];
    pipe(p);
    
    int pid = fork();
    
    if (pid == -1) { // Fork error
        return 0;
    }
    
    else if (pid != 0) { // Parent process
        dup2(p[0], 0); // Parent process reads from child process' output
        close(p[0]);
        close(p[1]);
        
        std::string str;
        std::string sender;
        std::string receiver;
        std::string message;
        int msgStart;
        
        // Wait for sniffed data from child process, tokenize, pass to verify()
        while (true) {
            std::getline(std::cin, str);
            sender = hostIP[str.substr(0, 15)];
            receiver = hostIP[str.substr(str.find('-', 0) + 1, 15)];
            msgStart = str.find(':', 0) + 2;
            message = str.substr(msgStart, str.length() - msgStart);
            
            // Append additional tcp segments until a newline is reached and the message is fully assembled
            while (true) {
                std::getline(std::cin, str);
                if (str == "") {
                    break;
                }
                message += str.substr(msgStart, str.length() - msgStart);
            }
            
            verify(sender, receiver, message);
            message = "";
        }
    }
    
    else { // Child process
        dup2(p[1], 1); // Child process writes to parent process' input
        close(p[0]);
        close(p[1]);
        
        // Set up and run sniffer
        char *argv[5];
        argv[0] = (char *) "tcpflow";
        argv[1] = (char *) "-i";
        argv[2] = &ifaceID[0u];
        argv[3] = (char *) "-c";
        argv[4] = NULL;
        execvp(argv[0], argv);
    }
    
}

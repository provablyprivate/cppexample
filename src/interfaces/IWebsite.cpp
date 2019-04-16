#include "./Constants.h"
#include "../Connection.h"


class IWebsite {
private:
    Connection *rWebsiteConnection;
    Connection *oChildConnection;
    Connection *oParentConnection;
    
    InterfaceHelper *helper;
    JSONHandler *parentJSON;
    
    Crypt *publicParentCrypt;
    
    bool validParentSignature;

    void oChildConnectionHandler() {
        Poco::Thread oChildConnectionThread;
        oChildConnectionThread.start(*oChildConnection);

        rWebsiteConnection->waitForEstablishment();
        oChildConnection->waitForEstablishment();
        
        std::string s; // pdata goes here
        while (true) {
            
            
            //decrypt etc, pass to RWebsite
            
            
        }
    }

public:
    IWebsite() {
        rWebsiteConnection = new Connection(I_INTERNAL_PORT);
        oChildConnection = new Connection(I_EXTERNAL_PORT_1);
        oParentConnection = new Connection(I_EXTERNAL_PORT_2);
        helper = new InterfaceHelper();
        publicParentCrypt = new Crypt("./src/rsa-keys/parent.pub");

    }

    void run() {
        Poco::Thread rWebsiteConnectionThread;
        rWebsiteConnectionThread.start(*rWebsiteConnection);

        Poco::RunnableAdapter<IWebsite> oChildFuncAdapt(*this, &IWebsite::oChildConnectionHandler);
        Poco::Thread oChildConnectionHandlerThread;
        oChildConnectionHandlerThread.start(oChildFuncAdapt);

        Poco::Thread oParentConnectionThread;
        oParentConnectionThread.start(*oParentConnection);

        rWebsiteConnection->waitForEstablishment();
        oParentConnection->waitForEstablishment();
        
        std::string s; // consent goes here
        while (true) {
            oParentConnection->waitForReceivedData();
            s = oParentConnection->getData();
            
            std::vector<std::string> messages = helper->splitString(s, 's');
            
            if (messages.size() != 2)
                continue;
            
            parentJSON = new JSONHandler(helper->decodeHex(messages[0]));
            std::string parentSignature = messages[1];
            
            validParentSignature = publicParentCrypt->verify(parentJSON->getObject(), parentSignature);
            
            if (!validParentSignature)
                continue;
            
            rWebsiteConnection->sendData(s);
            // decrypt, verify signature etc, pass to RWebsite
            
            
        }
    }


};

int main() {
    //if (DEBUG) freopen("./errorlogIW.txt", "a", stdout);
    IWebsite iWebsite;
    iWebsite.run();
}

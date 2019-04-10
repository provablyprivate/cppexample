#include "Constants.h"
#include "Connection.h"
#include "./crypt.cpp"
#include "./jsonhandler.cpp"
#include <regex>

class OChild {

private:
    Connection *rChildConnection;
    Connection *iWebsiteConnection;
    Connection *oWebsiteConnection;
    Crypt * privateChildCrypt;
    Crypt * publicParentCrypt;
    Crypt * publicWebsiteCrypt;
    JSONHandler * childJSON;

    bool verifySignature(Crypt* key, string signature, Object::Ptr json) {
        return key->verify(json, signature);
    }

    void rChildConnectionHandler() {
        rChildConnection->waitForEstablishment();
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);

        std::string s;
        while (true) {
            rChildConnection->waitForReceivedData();
            s = rChildConnection->getData();
            // assemble term
            std::cout << "Received from RChild: " << s << std::endl;
            iWebsiteConnection->sendData(s);
        }
    }

    void oWebsiteConnectionHandler() {
        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;
        }
    }

    void oWebsiteHandler() {
        oWebsiteConnection->waitForEstablishment();
        Poco::Thread oWebsiteConnectionThread;
        oWebsiteConnectionThread.start(*oWebsiteConnection);

        std::string s;
        while (true) {
            oWebsiteConnection->waitForReceivedData();
            s = oWebsiteConnection->getData();
            std::cout << "Received from OWebsite: " << s << std::endl;

            std::regex regex ("([\\s\\S]*)\n----BEGIN SIGNATURE----\n([\\s\\S]*)");   // matches words beginning by "sub"
            std::smatch match;
            regex_search(s, match, regex);

            JSONHandler * previousJSON = new JSONHandler(match[1]); // creating JSON from the parsed string
            std::string previousSignature = match[2];               // creating signature from the parsed string

            childJSON->put("PrevJson", previousJSON->getObject());  // Get the object and puts it in the JSON
            childJSON->put("PrevSign", previousSignature);          // Puts the signature

            //*update flag*
            //*notify thread*
        }
    }

    void rChildHandler() {
        rChildConnection->waitForEstablishment();
        Poco::Thread rChildConnectionThread;
        rChildConnectionThread.start(*rChildConnection);

        std::string s;
        while (true) {
            rChildConnection->waitForReceivedData();
            s = rChildConnection->getData();
            std::cout << "Received from RChild: " << s << std::endl;

            string encryptedData = publicWebsiteCrypt->encrypt(s);  // encrypts the data
            childJSON->put("Value", encryptedData);                 // Puts it in the JSON

            //*update flag*
            //*notify thread*
        }
    }

public:
    OChild(std::string websiteIP) {
        rChildConnection = new Connection(O_INTERNAL_PORT);
        iWebsiteConnection = new Connection(websiteIP, I_EXTERNAL_PORT_1);
        oWebsiteConnection = new Connection(O_EXTERNAL_PORT_1);
        privateChildCrypt = new Crypt("./rsa-keys/child", "./rsa-keys/child.pub");
        publicParentCrypt = new Crypt("./rsa-keys/parent.pub");
        publicWebsiteCrypt = new Crypt("./rsa-keys/website.pub");
        childJSON = new JSONHandler();
    }

    void run() {

        Poco::RunnableAdapter<OChild> rChildFuncAdapt(*this, &OChild::rChildConnectionHandler);
        Poco::Thread rChildConnectionHandlerThread;
        rChildConnectionHandlerThread.start(rChildFuncAdapt);

        /*Poco::RunnableAdapter<OChild> iWebsiteFuncAdapt(*this, &OChild::iWebsiteConnectionHandler);
        Poco::Thread iWebsiteConnectionHandlerThread;
        iWebsiteConnectionHandlerThread.start(iWebsiteFuncAdapt);*/

        Poco::RunnableAdapter<OChild> oWebsiteFuncAdapt(*this, &OChild::oWebsiteConnectionHandler);
        Poco::Thread oWebsiteConnectionHandlerThread;
        oWebsiteConnectionHandlerThread.start(oWebsiteFuncAdapt);

        iWebsiteConnection->waitForEstablishment();
        Poco::Thread iWebsiteConnectionThread;
        iWebsiteConnectionThread.start(*iWebsiteConnection);
    }

};

int main(int argc, char **argv) {
    OChild oChild(argv[2]);
    oChild.run();


    rchildhandler
    oWebsitehandler
    checkifjsoncomplete()

    return 0;
}


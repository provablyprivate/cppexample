#include "Constants.h"
#include "Connection.h"


class RWebsite {
private:
    Connection *websiteParentConnection;
    Connection *websiteChildConnection;
    Connection *iChildConnection;
    Connection *oChildConnection;
    
    RWebsite(int parentPort, int childPort) {
        
    }
};


int main(int argc, char **argv) {
    RWebsite rWebsite(argv[2], argv[3]);
    rWebsite.run();
    
    return 0;
}

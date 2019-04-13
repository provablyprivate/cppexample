#ifndef SRC_INTERFACES_INTERFACEHELPER_H_
#define SRC_INTERFACES_INTERFACEHELPER_H_
#include <sstream>
#include <vector>
#include "Poco/HexBinaryDecoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/StreamCopier.h"

using namespace std;

class InterfaceHelper {
 private:
    vector<bool> flags;

 public:
    InterfaceHelper();
    ~InterfaceHelper();
    explicit InterfaceHelper(int flagSize);

    string encodeHex(string input);
    string decodeHex(string input);
    vector<string> splitString(string input, char delimiter);

    bool all();
    void set(int position, bool value);
    void clear();
};

#endif /* SRC_INTERFACES_INTERFACEHELPER_H_ */

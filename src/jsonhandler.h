#ifndef SRC_JSONHANDLER_H_
#define SRC_JSONHANDLER_H_

#include "Poco/JSON/Parser.h"

using std::string;
using Poco::Dynamic::Var;

class JSONHandler {
 private:
    Poco::JSON::Parser parser;
    Var variable;

 public:
    JSONHandler();
    explicit JSONHandler(string input);

    void put(string key, string value);
    Var getVar(string data);
    string get(string data);
    string toString();
};

#endif  // SRC_JSONHANDLER_H_

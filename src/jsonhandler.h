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

    void put(string key, Var value);
    Var get(string data);
    string toString();
    Poco::JSON::Object::Ptr getObject();
};

#endif  // SRC_JSONHANDLER_H_

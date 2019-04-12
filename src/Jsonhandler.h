// See jsonhandler.cpp for detailed description of variables and functions

#ifndef SRC_JSONHANDLER_H_
#define SRC_JSONHANDLER_H_
#include "Poco/JSON/Parser.h"

using std::string;
using Poco::Dynamic::Var;
using Poco::JSON::Object;

class JSONHandler {
 private:
    Poco::JSON::Parser parser;
    Object::Ptr object;

 public:
    JSONHandler();
    explicit JSONHandler(string input);
    explicit JSONHandler(Object::Ptr input);

    void put(string key, Var value);
    Var get(string data);
    string toString();
    Object::Ptr getObject();
};

#endif  // SRC_JSONHANDLER_H_

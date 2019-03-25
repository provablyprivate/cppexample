#ifndef SRC_JSONHANDLER_H_
#define SRC_JSONHANDLER_H_

#include <vector>
#include <string>
#include "Poco/JSON/Parser.h"
// #include "Poco/JSON/Object.h"
// #include "Poco/JSON/ParseHandler.h"
// #include "Poco/JSON/Handler.h"
// #include "Poco/Dynamic/Var.h"
// #include "Poco/JSON/Object.h"
// #include <iostream>
// Why dont i need these above???
// Is it because they're included in the imported header?

class JSONHandler {
 private:
    Poco::JSON::Parser parser;
    Poco::JSON::Object::Ptr object;

 public:
    explicit JSONHandler(std::string input);
    std::string getType();
    std::string getData();
    std::string getConsent();
    std::string getChild();
    std::vector<std::string> getAll();

    void putType(std::string value);
    void putData(std::string value);
    void putConsent(std::string value);
    void putChild(std::string value);
    //  All get functions semi-hardcoded. Bad idea?
    //  Maybe have a generic function that returns
    //  specified key(if available)?
    explicit JSONHandler(Poco::JSON::Object::Ptr parsedResult);
    Poco::JSON::Object::Ptr getObject();
};

#endif  // SRC_JSONHANDLER_H_

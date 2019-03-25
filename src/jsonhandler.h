#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "Poco/JSON/Parser.h"
//#include "Poco/JSON/Object.h"
//#include "Poco/JSON/ParseHandler.h"
//#include "Poco/JSON/Handler.h"
//#include "Poco/Dynamic/Var.h"
//#include "Poco/JSON/Object.h"
//#include <iostream>
//#include <string>
// Why dont i need these above???
// Is it because they're included in the imported header?
// Is it still good practice to include the usual suspects like string and iostream?

class JSONHandler
{
private:
    Poco::JSON::Parser parser;
    Poco::JSON::Object::Ptr object;

public:

    JSONHandler(std::string input);
    std::string getType();
    std::string getData();
    std::string getConsent();
    std::string getChild();
    std::vector<std::string> getAll();

    void putType(std::string value);
    void putData(std::string value);
    void putConsent(std::string value);
    void putChild(std::string value);
    //All get functions semi-hardcoded. Bad idea?
    //Maybe have a generic function that returns
    //specified key(if available)?
    JSONHandler(Poco::JSON::Object::Ptr parsedResult);
    Poco::JSON::Object::Ptr getObject();
};

#endif /* JSONHANDLER_H */

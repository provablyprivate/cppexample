#include <string>
#include "./jsonhandler.h"

JSONHandler::JSONHandler(string input) {
    variable = parser.parse(input);
}

JSONHandler::JSONHandler() {
    variable = parser.parse("{}");
}

// Sets a new value
void JSONHandler::put(string key, Var value) {
    Poco::JSON::Object::Ptr obj = variable.extract<Poco::JSON::Object::Ptr>();
    obj->set(key, value);
}

// Returns the dynamic variable
Var JSONHandler::get(string data) {
    Poco::JSON::Object::Ptr obj = variable.extract<Poco::JSON::Object::Ptr>();
    return obj->get(data);
}

string JSONHandler::toString() {
    return variable.convert<std::string>();
}


#include <string>
#include "./jsonhandler.h"

JSONHandler::JSONHandler(string input) {
    variable = parser.parse(input);
}

JSONHandler::JSONHandler() {
    variable = parser.parse("{}");
}

// Sets a new value
void JSONHandler::put(string key, string value) {
    Poco::JSON::Object::Ptr obj = variable.extract<Poco::JSON::Object::Ptr>();
    obj->set(key, value);
}


// Returns the dynamic variable
Var JSONHandler::getVar(string data) {
    Poco::JSON::Object::Ptr obj = variable.extract<Poco::JSON::Object::Ptr>();
    return obj->get(data);
}

// Returns the variable as a string
string JSONHandler::get(string data) {
    Poco::JSON::Object::Ptr obj = variable.extract<Poco::JSON::Object::Ptr>();
    return obj->get(data).toString();
}

string JSONHandler::toString() {
    return variable.toString();
}

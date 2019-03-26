#include <string>
#include "./jsonhandler.h"

JSONHandler::JSONHandler(std::string input) {
    object = parser.parse(input).extract<Poco::JSON::Object::Ptr>();
}

JSONHandler::JSONHandler(Poco::JSON::Object::Ptr parsedResult) {
    object = parsedResult;
}
// Returns the TYPE value of the JSON
std::string JSONHandler::getType() {
    return object->get("Type").toString();
    //  behövs this->object???
}

// Sets a new value for TYPE
void JSONHandler::putType(std::string value) {
    object->set("Type", value);
}

#include <string>
#include "./jsonhandler.h"

JSONHandler::JSONHandler(string input) {
    object = parser.parse(input).extract<Object::Ptr>();
}

JSONHandler::JSONHandler(Object::Ptr input) {
    object = input;
}

JSONHandler::JSONHandler() {
    object = parser.parse("{}").extract<Object::Ptr>();
}

// Sets a new value
void JSONHandler::put(string key, Var value) {
    object->set(key, value);
}

// Returns the dynamic variable
Var JSONHandler::get(string data) {
    return object->get(data);
}

string JSONHandler::toString() {
    Poco::DynamicStruct ds = *object;
    return ds.toString();
}

Poco::JSON::Object::Ptr JSONHandler::getObject() {
	return object;
}

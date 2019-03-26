#include <string>
#include "./jsonhandler.h"

JSONHandler::JSONHandler(std::string input) {
    object = parser.parse(input).extract<Poco::JSON::Object::Ptr>();
}

JSONHandler::JSONHandler(Poco::JSON::Object::Ptr parsedResult) {
    object = parsedResult;
}
// Returns the Consent value of the JSON
std::string JSONHandler::getConsent() {
    return object->get("Consent").toString();
}

// Sets a new value for Consent
void JSONHandler::putConsent(std::string value) {
    object->set("Consent", value);
}

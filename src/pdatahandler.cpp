#include "jsonhandler.h"

JSONHandler::JSONHandler(std::string input)
{
    object = parser.parse(input).extract<Poco::JSON::Object::Ptr>();
}

JSONHandler::JSONHandler(Poco::JSON::Object::Ptr parsedResult)
{
    object = parsedResult;
}
// Returns the Data value of the JSON
std::string JSONHandler::getData()
{
    return object->get("Data").toString();
}

// Sets a new value for Data
void JSONHandler::putData(std::string value)
{
    object->set("Data",value);
}

//NOT DONE SINCE DATA CAN BE ENCRYPTED

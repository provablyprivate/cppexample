#include "../jsonhandler.h"

JSONHandler::JSONHandler(std::string json)
{
    object = parser.parse(json).extract<Poco::JSON::Object::Ptr>();
}

Poco::JSON::Object::Ptr JSONHandler::getObject()
{
    return object;
}

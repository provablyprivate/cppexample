#include <iostream>
#include <string>
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Handler.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "jsonparser.h"

class JSONParser
{

    Poco::JSON::Parser parser;
    Poco::JSON::Object::Ptr key;

    public:

    JSONParser(std::string input)
    {
        key = parser.parse(input).extract<Poco::JSON::Object::Ptr>();
    }
    
    //Returns the TYPE value of the JSON
    std::string getType()
    {
        std::string typeValue = key->get("Type").convert<std::string>();
        return typeValue;
    }

    //Returns the DATA value of the JSON
    std::string getData()
    {
        std::string dataValue = key->get("Data").convert<std::string>();
        return dataValue;
    }
    
    //Returns the CONSENT value of the JSON
    std::string getConsent()
    {
        std::string consentValue = key->get("Consent").convert<std::string>();
        return consentValue;
    }
    

    //Returns the CHILD value of the JSON
    std::string getChild()
    {
        std::string childValue = key->get("Child").convert<std::string>();
        return childValue;
    }

    //Returns ALL values of the JSON in an VECTOR ARRAY
    std::vector<std::string> getAll()
    {
        std::vector<std::string> allValues;
        allValues.push_back(getType());
        allValues.push_back(getData());
        allValues.push_back(getConsent());
        allValues.push_back(getChild());
        return allValues;
    }

    

};

int main(){
    std::string json = "{ \"Type\" : \"Policy\", \"Data\" : \"SECRET\", \"Consent\" : \"YES\", \"Child\" : \"27\" }";
    JSONParser* jsonparser = new JSONParser(json);

    std::cout << "Printing functions individually: \n";
    std::cout << jsonparser->getType() << "\n";
    std::cout << jsonparser->getData() << "\n";
    std::cout << jsonparser->getConsent() << "\n";
    std::cout << jsonparser->getChild() << "\n\n";

    std::cout << "Print the values of the vector array: \n";
    for (int i=0; i<4; i++){
        std::cout << jsonparser->getAll()[i] << "\n";
    }

    return 0;
}

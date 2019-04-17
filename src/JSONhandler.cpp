#include "./JSONhandler.h"

#include <string>

/*! \brief Parses a string to a JSON object
 *
 * The input is a string representation of a JSON. This gets fed to the parse instance
 * derived from the Poco library. If successful, a JSON object will be extracted
 * and set.
 */
JSONHandler::JSONHandler(string input) {
    object = parser.parse(input).extract<Object::Ptr>();
}

/*! \brief Sets the JSON object input as the active object.
 *
 * Used primarily when the structure of the JSON is known, e.g. when creating
 * nested JSON objects.
 */
JSONHandler::JSONHandler(Object::Ptr input) {
    object = input;
}

/*! \brief Creates a new, empty JSON object
 *
 * The string parsed are two empty brackets that represent an empty JSON.
 * This is required by Pocos parse class.
 */
JSONHandler::JSONHandler() {
    object = parser.parse("{}").extract<Object::Ptr>();
}

/*! \brief Updates the JSON with the specified key and value
 *
 * The value will be interpreted, and set, as a Dynamic::Var
 * according to the Poco library. If the key already exists it overwrites
 * the old value with the new one. If the key does not exist, a new field
 * is inserted in the JSON with the key/value pair.
 */
void JSONHandler::put(string key, Var value) {
    object->set(key, value);
}

/*! \brief Gets the value of the key input
 *
 *  An empty value is returned if the property does not exist.
 *
 * \return Poco::Dynamic::Var value
 */
Var JSONHandler::get(string data) {
    return object->get(data);
}

/*! \brief Converts the JSON object to a string and returns it
 *
 * Creates a DynamicStruct to the pointer value of the object and uses to built
 * in function 'toString()' to convert it.
 *
 * \return std::string object
 */
string JSONHandler::toString() {
    Poco::DynamicStruct ds = *object;
    return ds.toString();
}

/*! \brief Returns the JSON object
 *
 * Used primarily together with the JSONHandler constructor accepting
 * JSON objects to create nested JSONs
 *
 * \return Poco::JSON::Object::Ptr object
 */
Object::Ptr JSONHandler::getObject() {
    return object;
}

/*! \brief Returns the JSON object as a hex string
 *
 * Poco::BinaryHexEncoder is used to convert a stringstream into a hex string.
 * The toString function is fed into the stringstream to be converted.
 * setLineLength is set to 0 in the encoder to remove the standard 72 character
 * length before a new line is inserted.
 *
 * \return std::string encoded.str()
 */
string JSONHandler::toHex() {
    std::ostringstream encoded;
    Poco::HexBinaryEncoder encoder(encoded);
    encoder.rdbuf()->setLineLength(0);
    encoder << toString();
    encoder.close();

    return encoded.str();
}

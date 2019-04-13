#include "./InterfaceHelper.h"

using namespace std;

InterfaceHelper::InterfaceHelper(int flagSize) {
    flags.resize(flagSize);
}

/*! \brief Returns the JSON object as a hex string
 *
 * Poco::BinaryHexEncoder is used to convert a stringstream into a hex string.
 * The toString function is fed into the stringstream to be converted.
 * setLineLength is set to 0 in the encoder to remove the standard 72 character
 * length before a new line is inserted.
 *
 * \return string encoded.str()
 */
string InterfaceHelper::encodeHex(string string) {
    ostringstream encoded;
    Poco::HexBinaryEncoder encoder(encoded);
    encoder.rdbuf()->setLineLength(0);
    encoder << string;
    encoder.close();

    return encoded.str();
}

/*! \brief Decodes a hex string.
 *
 * Decodes a hex string and returns the result.
 *
 * \return string decoded
 */
string InterfaceHelper::decodeHex(string input) {
    string decoded;
    istringstream istream(input);
    Poco::HexBinaryDecoder decoder(istream);
    Poco::StreamCopier::copyToString(decoder, decoded);

    return decoded;
}

/*! \brief Splits string given a delimiter
 *
 * Used to split the incoming hex messages separated by a dot character.
 *
 * \return vector<string> matches
 */
vector<string> InterfaceHelper::splitString(string input, char delimeter) {
    istringstream ss(input);
    string match;
    vector<string> matches;
    while (getline(ss, match, delimeter)) {
       matches.push_back(match);
    }

    return matches;
}

/*! \brief Check if all flags are set in bool vector
 *
 * \return bool
 */
bool InterfaceHelper::all() {
    for (unsigned int i = 0; i < flags.size(); i++) {
       if (!flags.at(i)) return false;
    }
    return true;
}

/* \brief Changed the boolean value at given index */
void InterfaceHelper::set(int position, bool value) {
    flags.at(position) = value;
}

/* \brief resets all values in falgs vector to false */
void InterfaceHelper::clear() {
    flags.assign(flags.size(), false);
}

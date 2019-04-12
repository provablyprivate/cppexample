#define SALT "hackerman"
#define DIGEST "sha1"
#define METHOD "AES-256-CBC"
#define ITERATIONS 100
#include "Poco/Crypto/Cipher.h"
Poco::Crypto::Cipher* createKey(std::string password, std::string salt, std::string digest, std::string method);
Poco::Crypto::Cipher* createKey(std::string password);

std::string decrypt(std::string a, Poco::Crypto::Cipher* cipher);
std::string decrypt(std::string a);

std::string encrypt(std::string a, Poco::Crypto::Cipher* cipher);
std::string encrypt(std::string a);

#include <iostream>

const std::string LOCALHOST = "127.0.0.1";

const int PORT_BASE         = 32000;
const int O_INTERNAL_PORT   = PORT_BASE + 1;
const int O_EXTERNAL_PORT_1 = PORT_BASE + 2;
const int O_EXTERNAL_PORT_2 = PORT_BASE + 3;
const int I_INTERNAL_PORT   = PORT_BASE + 4;
const int I_EXTERNAL_PORT_1 = PORT_BASE + 5;
const int I_EXTERNAL_PORT_2 = PORT_BASE + 6;

const std::string connectionTerminator = "bye";

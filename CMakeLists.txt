cmake_minimum_required(VERSION 3.4)
project(provablyprivate)

set(CMAKE_CXX_STANDARD 17)
include_directories("include")

find_package (Threads)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(Poco REQUIRED COMPONENTS Net NetSSL Data JSON Crypto Foundation)
message(INFO, ${Poco_INCLUDE_DIRS})
message(INFO, ${Poco_LIBRARIES})


set(CLIENT_SOURCE src/client.cpp)
add_executable(client ${CLIENT_SOURCE})

set(SERVER_SOURCE src/server.cpp)
add_executable(server ${SERVER_SOURCE})



target_link_libraries(client
    ${CMAKE_THREAD_LIBS_INIT}
    ${Poco_LIBRARIES}
)
target_link_libraries(server
    ${CMAKE_THREAD_LIBS_INIT}
    ${Poco_LIBRARIES}
)
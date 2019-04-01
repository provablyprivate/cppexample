cmake_minimum_required(VERSION 3.4)
project(cppexample)

set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    include_directories("include")

    find_package (Threads)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
    find_package(Poco REQUIRED COMPONENTS Net NetSSL Data JSON Crypto Foundation)
    message(INFO, ${Poco_INCLUDE_DIRS})
    message(INFO, ${Poco_LIBRARIES})

    set(LIBS ${CMAKE_THREAD_LIBS_INIT} ${Poco_LIBRARIES})

endif()

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    # the directory containing "include" and "lib"
    set(POCO_PREFIX /usr/local/opt/poco)
    set(OPENSSL_PREFIX /usr/local/opt/openssl)

    include_directories(${POCO_PREFIX}/include)
    include_directories(${OPENSSL_PREFIX}/include)
    link_directories(${POCO_PREFIX}/lib)
    link_directories(${OPENSSL_PREFIX}/lib)

    file(GLOB POCO_LIBRARIES "${POCO_PREFIX}/lib/*.dylib")
    file(GLOB OPENSSL_LIBRARIES "${OPENSSL_PREFIX}/lib/*.dylib")
    message("POCO_LIBRARIES = ${POCO_LIBRARIES}")
    message("OPENSSL_LIBRARIES = ${OPENSSL_LIBRARIES}")

    set(LIBS ${POCO_LIBRARIES} ${OPENSSL_LIBRARIES})

endif()

add_executable(client src/client.cpp)
add_executable(server src/server.cpp)
add_executable(json test/json_test.cpp)
add_executable(com test/communication/com-test.cpp)

target_link_libraries(client ${LIBS})
target_link_libraries(server ${LIBS})
target_link_libraries(json ${LIBS})
target_link_libraries(com ${LIBS})

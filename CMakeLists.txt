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
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    # the directory containing "include" and "lib"
    set(POCO_PREFIX /usr/local/Cellar/poco/1.9.0_1)
    set(OPENSSL_PREFIX /usr/local/Cellar/openssl/1.0.2q)

    include_directories(${POCO_PREFIX}/include)
    include_directories(${OPENSSL_PREFIX}/include)
    link_directories(${POCO_PREFIX}/lib)
    link_directories(${OPENSSL_PREFIX}/lib)

    file(GLOB POCO_LIBRARIES "${POCO_PREFIX}/lib/*.dylib")
    file(GLOB OPENSSL_LIBRARIES "${OPENSSL_PREFIX}/lib/*.dylib")
    message("POCO_LIBRARIES = ${POCO_LIBRARIES}")
    message("OPENSSL_LIBRARIES = ${OPENSSL_LIBRARIES}")

    add_executable(client src/client.cpp)
    add_executable(server src/server.cpp)

    target_link_libraries(client
        ${POCO_LIBRARIES}
        ${OPENSSL_LIBRARIES}
    )
    target_link_libraries(server
        ${POCO_LIBRARIES}
        ${OPENSSL_LIBRARIES}
    )
endif()
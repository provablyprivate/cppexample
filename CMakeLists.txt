cmake_minimum_required(VERSION 3.4)
project(cppexample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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


add_executable(Parent src/Parent.cpp)
add_executable(Child src/Child.cpp)
add_executable(Website src/Website.cpp)
add_executable(com test/communication/com-test.cpp)
add_executable(RChild src/interfaces/RChild.cpp)
add_executable(OChild src/interfaces/OChild.cpp)
add_executable(RWebsite src/interfaces/RWebsite.cpp)
add_executable(IWebsite src/interfaces/IWebsite.cpp)
add_executable(OWebsite src/interfaces/OWebsite.cpp)
add_executable(IParent src/interfaces/IParent.cpp)
add_executable(OParent src/interfaces/OParent.cpp)
add_executable(RParent src/interfaces/RParent.cpp)

add_executable(ThirdParty src/ThirdParty.cpp)


target_link_libraries(Parent ${LIBS})
target_link_libraries(Child ${LIBS})
target_link_libraries(Website ${LIBS})
target_link_libraries(RChild ${LIBS})
target_link_libraries(com ${LIBS})
target_link_libraries(OChild ${LIBS})
target_link_libraries(RWebsite ${LIBS})
target_link_libraries(IWebsite ${LIBS})
target_link_libraries(OWebsite ${LIBS})
target_link_libraries(IParent ${LIBS})
target_link_libraries(OParent ${LIBS})
target_link_libraries(RParent ${LIBS})

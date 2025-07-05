cmake_minimum_required(VERSION 3.10)
project(WebSocketServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Find libwebsockets
pkg_check_modules(LIBWEBSOCKETS REQUIRED libwebsockets)

# Find OpenSSL (required by libwebsockets)
find_package(OpenSSL REQUIRED)

# Find nlohmann-json
pkg_check_modules(NLOHMANN_JSON REQUIRED nlohmann_json)

# Find libcurl
pkg_check_modules(LIBCURL REQUIRED libcurl)

add_executable(server server.cpp)

target_include_directories(server PRIVATE
    ${LIBWEBSOCKETS_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIRS}
    ${LIBCURL_INCLUDE_DIRS}
)

target_link_libraries(server PRIVATE
    Threads::Threads
    ${LIBWEBSOCKETS_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${NLOHMANN_JSON_LIBRARIES}
    ${LIBCURL_LIBRARIES}
)

target_link_directories(server PRIVATE
    ${LIBWEBSOCKETS_LIBRARY_DIRS}
    ${NLOHMANN_JSON_LIBRARY_DIRS}
    ${LIBCURL_LIBRARY_DIRS}
) 
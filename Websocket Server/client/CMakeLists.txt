cmake_minimum_required(VERSION 3.10)
project(WebSocketClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Find libwebsockets
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWEBSOCKETS REQUIRED libwebsockets)

# Find OpenSSL (required by libwebsockets)
find_package(OpenSSL REQUIRED)

# Create executable
add_executable(client client.cpp)

# Include directories
target_include_directories(client PRIVATE
    ${LIBWEBSOCKETS_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(client PRIVATE
    Threads::Threads
    ${LIBWEBSOCKETS_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

# Link directories
target_link_directories(client PRIVATE
    ${LIBWEBSOCKETS_LIBRARY_DIRS}
) 
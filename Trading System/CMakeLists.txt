cmake_minimum_required(VERSION 3.10)
project(TradingSystem)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JsonCpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Find nlohmann-json
find_package(nlohmann_json REQUIRED)

# Add executable
add_executable(trading_system main.cpp helper.cpp)

# Include directories
target_include_directories(trading_system PRIVATE ${JSONCPP_INCLUDE_DIRS})

# Link libraries
target_link_libraries(trading_system ${JSONCPP_LIBRARIES} nlohmann_json::nlohmann_json)

# Add compiler flags
target_compile_options(trading_system PRIVATE ${JSONCPP_CFLAGS_OTHER}) 
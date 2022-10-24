cmake_minimum_required(VERSION 3.0)
include(FindPkgConfig)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cp-server VERSION 0.1
DESCRIPTION "This is a project that will be used to remotely power on a pc"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(cpprestsdk REQUIRED CONFIG)

pkg_check_modules(SSL openssl REQUIRED)

file(GLOB_RECURSE cp-server_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE cp-server_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_executable(cp-server ${cp-server_SOURCES})

target_include_directories(cp-server PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(cp-server cpprestsdk::cpprest ${SSL_LINK_LIBRARIES})

install(TARGETS cp-server RUNTIME DESTINATION bin)

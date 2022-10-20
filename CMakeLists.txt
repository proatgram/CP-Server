cmake_minimum_required(VERSION 3.0)

project(cp-server)

add_executable(cp-server main.cpp)

install(TARGETS cp-server RUNTIME DESTINATION bin)

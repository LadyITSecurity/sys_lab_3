cmake_minimum_required(VERSION 3.21)
project(sys_lab_3)

set(CMAKE_CXX_STANDARD 14)

add_executable(sys_lab_3 client_main.cpp common.h check.hpp server_main.cpp)

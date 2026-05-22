cmake_minimum_required(VERSION 3.15)
project(CSOPESY-MO1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(csopesy_mo1 ${SOURCES})

target_include_directories(csopesy_mo1 PRIVATE include)

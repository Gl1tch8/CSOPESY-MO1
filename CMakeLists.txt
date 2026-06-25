cmake_minimum_required(VERSION 3.20)

project(CSOPESY_MO1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CONSOLE_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER CONSOLE_SOURCES EXCLUDE REGEX "/gui/")

add_executable(csopesy_mo1
    ${CONSOLE_SOURCES}
)

if(MSVC)
    target_compile_options(csopesy_mo1 PRIVATE /FS)
endif()

target_include_directories(csopesy_mo1 PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/misc"
    "${PROJECT_SOURCE_DIR}/src"
)
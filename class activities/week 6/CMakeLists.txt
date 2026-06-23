cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Name of the project
project(Week6Activities CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# Require C++17 standard (Change to 11, 14, or 20 if your project requires differently)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define all the source files (.cpp)
set(SOURCES
    src/main.cpp
    src/ConsoleManager.cpp
    src/MainConsole.cpp
    src/Process.cpp
    src/Scheduler.cpp
    src/Screen.cpp
    src/instructions/IAdd.cpp
    src/instructions/IDeclare.cpp
    src/instructions/IFor.cpp
    src/instructions/IPrint.cpp
    src/instructions/ISleep.cpp
    src/instructions/ISubtract.cpp
)

# Create the executable
add_executable(Week6App ${SOURCES})

# Include directories so the compiler can find your header (.h) files
target_include_directories(Week6App PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instructions
)

# Copy the config.txt file to the build directory so the executable can read it at runtime
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/config.txt 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
cmake_minimum_required(VERSION 3.15)
project(CSOPESY_Emulator)
set(CMAKE_CXX_STANDARD 20)

add_executable(
    app

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

target_include_directories(app PRIVATE src src/instructions)

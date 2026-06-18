#pragma once

#include "globals.h"
#include "Console.h"
#include "MainConsole.h"
#include "Screen.h"

class Console;

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";

using ConsoleTable = std::unordered_map<std::string, std::shared_ptr<Console>>;

class ConsoleManager {
    public:
        static void initialize();
        static ConsoleManager* getInstance();
        static void destroy();
    
        void start();
        void stop();
        bool isRunning();
        
    private:
        ConsoleManager();
        ~ConsoleManager() = default;

        void run();
        bool registerScreen(std::string consoleName, bool isSwitch);
        void switchConsole(std::string consoleName);
        void terminate();

        bool running = false;
        static ConsoleManager* singleton;
        std::thread consoleThread;
        ConsoleTable consoleTable;
        std::shared_ptr<Console> currentConsole;

        friend class MainConsole;
        friend class Screen;
        friend class Scheduler;
};

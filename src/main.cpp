#include <stdio.h>
#include <iostream>
#include <string>
#include <format>

#include "include.hpp"



int main()
{
    std::string input;
    std::string command;
   
    Helper helper;
    
    InitializeService* initializeService = new InitializeService(); 
    InitializeCommand* initializeCommand = new InitializeCommand(initializeService);
    ScreenService* screenService = new ScreenService();
    ScreenCommand* screenCommand = new ScreenCommand(screenService);    ScreenMuxService* screenMuxService = new ScreenMuxService();
    SchedulerService* schedulerService = new SchedulerService();
    SchedulerCommand* schedulerCommand = new SchedulerCommand(schedulerService);
    ReportUtilService* reportUtilService = new ReportUtilService();
    ReportUtilCommand* reportUtilCommand = new ReportUtilCommand(reportUtilService);
    ClearService* clearService = new ClearService();
    ClearCommand* clearCommand = new ClearCommand(clearService);
    ExitService* exitService = new ExitService();
    ExitCommand* exitCommand = new ExitCommand(exitService);

    SystemState &state = SystemState::getInstance();


    // loop starting here:
    bool isRunning = true;
    MainMenuTUI::printMainMenu();
    while (isRunning) {
        std::cout << "root:\\>";
        std::getline(std::cin, input);
        if (!std::cin) {                 // EOF / closed stdin: stop instead of spinning
            state.stop();
            isRunning = false;
            break;
        }
        command = helper.parse(input);

        if (!SystemState::getInstance().isInitialized()) {
            if (command == "initialize") {
                initializeCommand->execute(input);
                state.start();          // begin ticking after init
            }
            else if (command == "exit") {
                exitCommand->execute(input);
                isRunning = false;
            }
            else {
                std::cout << "Error: 'initialize' must be called before any other command is recognized." << std::endl;
                continue; // Skip the rest of the loop and ask for input again
            }
        }
        else if (command == "initialize") {
            std::cout << "System is already initialized." << std::endl;
        }
        else if (command == "screen") {
            screenCommand->execute(input);

        }
        else if (command == "scheduler-start") {

            schedulerCommand->executeStart(input);
        }
        else if (command == "scheduler-stop") {

            schedulerCommand->executeStop(input);
        }
        else if (command == "report-util") {
            reportUtilCommand->execute(input);
        } else if (command == "exit") {

            state.stop();               // signal + join the tick thread before exiting
            exitCommand->execute(input);
			isRunning = false;
        }
    }
}



#include <stdio.h>
#include <iostream>
#include <string>
#include <format>

#include "include/include.hpp"

int main()
{


    // create variable for the header text
    std::string asciiArt = R"(
 _______  _______  _______  _______  _______  _______          
(  ____ \(  ____ \(  ___  )(  ____ )(  ____ \(  ____ \|\     /|
| (    \/| (    \/| (   ) || (    )|| (    \/| (    \/( \   / )
| |      | (_____ | |   | || (____)|| (__    | (_____  \ (_) / 
| |      (_____  )| |   | ||  _____)|  __)   (_____  )  \   /  
| |            ) || |   | || (      | (            ) |   ) (   
| (____/\/\____) || (___) || )      | (____/\/\____) |   | |   
(_______/\_______)(_______)|/       (_______/\_______)   \_/   
                                                              )";


    std::string headerText = asciiArt + "\nHello, Welcome to CSOPESY commandline!\nType 'exit' to quit, 'clear' to clear the screen";
    headerText += "\n\n** IMPORTANT: Type 'initialize' to load config and start system **\n\n";
    
    std::cout << headerText << std::endl;
    
    std::string input;
    std::string command;
    
    Helper helper;
    
        InitializeService* initializeService = new InitializeService(); 
        InitializeCommand* initializeCommand = new InitializeCommand(initializeService);
ScreenService* screenService = new ScreenService();
        ScreenCommand* screenCommand = new ScreenCommand(screenService);
SchedulerService* schedulerService = new SchedulerService();
        SchedulerCommand* schedulerCommand = new SchedulerCommand(schedulerService);
ReportUtilService* reportUtilService = new ReportUtilService();
        ReportUtilCommand* reportUtilCommand = new ReportUtilCommand(reportUtilService);
ClearService* clearService = new ClearService();
        ClearCommand* clearCommand = new ClearCommand(clearService);
ExitService* exitService = new ExitService();
        ExitCommand* exitCommand = new ExitCommand(exitService);



    // loop starting here:
    bool isRunning = true;
    while (isRunning) {
        std::cout << "Enter a command: ";
        std::getline(std::cin, input);
        command = helper.parse(input);


        if (command == "initialize") {
            initializeCommand->execute(input);
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
        }
        else if (command == "clear") {
            clearCommand->execute(input);
            std::cout << headerText << std::endl;
        }
        else if (command == "exit") {

            exitCommand->execute(input);
            std::cout << "Exiting..." << std::endl;
			isRunning = false;
        }
    }
}



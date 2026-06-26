#include <string>
#include <iostream>
#include "tui/MainMenu.hpp"
        void MainMenuTUI::printMainMenu() {
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
    headerText += "\nDevelopers:\nAvelino, Sophia\nChua, Renzo\nLiong, John\n";
    headerText += "\n\n** IMPORTANT: Type 'initialize' to load config and start system **\n\n";
    
            std::cout << headerText << std::endl;

    
        };

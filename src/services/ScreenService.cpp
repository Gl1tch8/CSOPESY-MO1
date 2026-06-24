#include "../include/services/ScreenService.hpp"
#include "../../include/misc/SystemState.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

ScreenService::ScreenService() : Service() {}

std::string ScreenService::executeFlags(std::string input) {
    std::stringstream ss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 2) {
        return "Error: screen command requires flags (e.g. screen -ls, screen -s <name>, screen -r <name>)";
    }

    std::string flag = tokens[1];

    if (flag == "-ls") {
       //todo
    } 
    else if (flag == "-s") {
        if (tokens.size() < 3) {
            return "Error: screen -s requires a process name (e.g. screen -s process_name)";
        }
        std::string name = tokens[2];
        

        
        //todo
    } 
    else if (flag == "-r") {
        if (tokens.size() < 3) {
            return "Error: screen -r requires a process name (e.g. screen -r process_name)";
        }
        std::string name = tokens[2];
        
        //todo
    }

    return "Error: Unrecognized screen flag '" + flag + "'";
}

std::string ScreenService::listProcesses() {
    SystemState &state = SystemState::getInstance();
    
    return "";
}

std::string ScreenService::newSessionProcess() {
    return "";
}

std::string ScreenService::reattachSessionProcess() {
    return "";
}

std::string ScreenService::openSessionWindow(std::string processName) {}

std::string ScreenService::reportUtil() {}


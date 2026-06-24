#pragma once
#include "Process.hpp"
#include "../../src/interfaces/Service.hpp"
#include <string>
#include <unordered_map>


class ScreenService : public Service {
public:

    ScreenService();

    // Reads the flags of the command and executes
    // the needed logic based on it
    // returns output log
    std::string executeFlags(std::string input);

    //screen -ls
    // prints out the ff:
    // CPU Utilization, cores used, cores available
    // running and finished processes 
    std::string listProcesses();

    //screen -ls but into a file
    std::string reportUtil();

    //screen -s <pid>
    // new screen session with process info
    std::string newSessionProcess();

    //screen -r <pid>
    // reattaches the screen session with process info 
    std::string reattachSessionProcess();

    std::string openSessionWindow(std::string processName);
    private:
        //stores the process name and the process details as a pointer
        std::unordered_map<std::string, Process*> screens;
};

class ScreenMuxService {
    public:
    // process smi command and return output log
        std::string processSMI();
    // prints process smi output
        std::string processSMIOutput();
    // exit screen mux
        std::string exit();


};
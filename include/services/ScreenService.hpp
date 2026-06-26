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
    std::string reattachSessionProcess(std::string processName);

    std::string openSessionWindow(std::string processName);
    
    // process screens
    bool hasActiveScreen() const;
    void clearActiveScreen();
    std::string getActiveScreen() const;

    private:
        //stores the process name and the process details as a pointer
        std::unordered_map<std::string, Process*> screens;
        std::string activeScreen;
        
        // executes the block of instructions for a process, updating  symbol table and tick count
        void executeBlock(const std::vector<Instruction>& instructions, SymbolTable& sym, uint64_t& tick, std::atomic<bool>& running);
        
};

class ScreenMuxService {
    public:
    // process smi command and return output log
        std::string processSMI(const std::string& processName);
    // prints process smi output
        std::string processSMIOutput();
    // exit screen mux
        std::string exit();


};
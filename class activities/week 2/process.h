// include only once in compilation
#pragma once

#include <ctime>
#include <string>
#include <vector>

//Q1
class Process {
public:
    enum class State { READY, RUNNING, FINISHED };

private:
    // Each process needs to be uniquely represented to make it easily
    // identifiable hence the attributes name, id and timestamp. 
    std::string name; 
    int         pid; 
    std::time_t createdAt; 

    // Core id is needed to know what cpu is being used for the process
    // for resource allocation 
    int         coreId; 

    // tracking state is needed to
    // check if the process requires resource allocation.
    State       state; 

    // list of instructions. Uses vector because it's dynamic.
    std::vector<std::string> instructions; 

    int currentLine; // index of the next line to run
    int totalLines; // total number of lines needed to execute

};

//Q2
// inherits Process class to gain process attributes
class InstructionProcess : public Process {
public:
    // allows the execution of the next line given the program counter
    void executeNext();
    // gets the index of the current line
    int  getCurrentLine();
    // gets the total number of lines in order to halt the program counter
    int  getTotalLines();
    // checks if the program is finished
    bool isFinished();
    // prints the current line
    void printLine();
};

#pragma once
#include "Service.hpp"
/**
 * The command class is an abstract class. 
 * This is used for invoking commands
 */
class Command {
public:
    // each command requires the service for that command
    Command(Service service);
    /**
     * Prints the output of the executed function to the command line
     */
    virtual void print();
    /**
     * Executes the command
     */
    virtual void execute();

};
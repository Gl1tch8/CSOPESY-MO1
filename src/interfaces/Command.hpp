#pragma once
#include <iostream>
#include <string>
#include "Service.hpp"

/**
 * The command class is an abstract class. 
 * This is used for invoking commands
 */
class Command {
public:
    Service& service;
    // each command requires the service for that command
    Command(Service& service) : service(service) {}
    /**
     * Prints the output of the executed function to the command line
     */
    virtual void print(std::string log) = 0;
    /**
     * Executes the command
     */
    virtual void execute(std::string input)=0;

};


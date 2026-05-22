#pragma once
#include <string>
/**
 * Service abstract class.
 * This class contains the logic behind the command.
 */
class Service {
public:
    /**
     * Reads the flags of the command and executes
     * the needed logic based on it
     * 
     */
    virtual void executeFlags(std::string input);
};


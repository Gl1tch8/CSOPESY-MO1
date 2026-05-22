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
     * returns output log
     */
    virtual std::string executeFlags(std::string input);
};


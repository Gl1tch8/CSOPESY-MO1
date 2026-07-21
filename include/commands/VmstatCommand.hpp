#pragma once
#include "../../src/interfaces/Command.hpp"
#include "../../include/services/VmstatService.hpp"

class VmstatCommand : public Command {
public:
    VmstatCommand(VmstatService* service);

    void print(std::string log);

    void execute(std::string input);
};

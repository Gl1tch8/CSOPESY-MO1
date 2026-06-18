#pragma once

#include "Instruction.h"

class ISubtract : public Instruction {
    public:
        ISubtract(uint16_t& var, uint16_t& val1, uint16_t& val2);

        void execute() override;
    
    private:
        uint16_t& var;
        uint16_t& val1;
        uint16_t& val2;
};
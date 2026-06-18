#pragma once

#include "Instruction.h"

class IAdd : public Instruction {
    public:
        IAdd(uint16_t& var, uint16_t& val1, uint16_t& val2);

        void execute() override;
    
    private:
        uint16_t& var;
        uint16_t& val1;
        uint16_t& val2;
};
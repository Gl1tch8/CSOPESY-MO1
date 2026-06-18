#pragma once

#include "Instruction.h"

class IDeclare : public Instruction {
    public:
        IDeclare(uint16_t& var, uint16_t val);

        void execute() override;
    private:
        uint16_t& var;
        uint16_t val;
};
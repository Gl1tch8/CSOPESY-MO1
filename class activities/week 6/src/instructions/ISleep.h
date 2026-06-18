#pragma once

#include "Instruction.h"

class ISleep : public Instruction {
    public:
        ISleep(uint8_t ticks);

        void execute() override;
        bool isLooping() override;

    private:
        uint8_t counter = 0;
        uint8_t ticks;
        bool isSleeping = true;
};
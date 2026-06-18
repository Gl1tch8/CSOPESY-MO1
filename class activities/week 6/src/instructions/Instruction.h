#pragma once

#include "globals.h"

class Instruction {
    public:
        ~Instruction() = default;
        
        virtual void execute() = 0;
        virtual bool isLooping() {
            return 0;
        }
};
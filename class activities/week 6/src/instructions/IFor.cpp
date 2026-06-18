#include "IFor.h"

IFor::IFor(std::unique_ptr<ForLoop> instructions, uint16_t repeats)
    : instructions(std::move(instructions)), repeats(repeats) 
{
    instructionSize = this->instructions->size();
}

void IFor::execute() {
    isRunning = true;

    (*instructions)[forIndex]->execute();

    if (!(*instructions)[forIndex]->isLooping()) {
        forIndex++;
    }

    if (forIndex == instructionSize) {
        forIndex = 0;
        repeatCount++;
    }

    if (repeatCount == repeats) {
        isRunning = false;
        repeatCount = 0;
    }
}

bool IFor::isLooping() {
    return isRunning;
}
#include "ISleep.h"

ISleep::ISleep(uint8_t ticks) : ticks(ticks) {}

void ISleep::execute() {
    counter++;
    if (counter == ticks) {
        isSleeping = false;
    }
    // std::cout << "Sleeping..." << std::endl;
}

bool ISleep::isLooping() {
    return isSleeping;
} 


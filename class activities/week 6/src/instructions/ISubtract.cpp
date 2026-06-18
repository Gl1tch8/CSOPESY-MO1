#include "ISubtract.h"

ISubtract::ISubtract(uint16_t& var, uint16_t& val1, uint16_t& val2)
    : var(var), val1(val1), val2(val2) {}

void ISubtract::execute() {
    var = val1 - val2;
}
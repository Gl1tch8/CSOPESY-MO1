#include "IDeclare.h"

IDeclare::IDeclare(uint16_t& var, uint16_t val)
    : var(var), val(val) {}

void IDeclare::execute() {
    var = val;
}
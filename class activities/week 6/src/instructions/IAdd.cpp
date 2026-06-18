#include "IAdd.h"

IAdd::IAdd(uint16_t& var, uint16_t& val1, uint16_t& val2) 
    : var(var), val1(val1), val2(val2) {}

void IAdd::execute() {
    var = val1 + val2;
}
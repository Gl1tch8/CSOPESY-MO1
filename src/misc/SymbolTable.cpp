#include "SymbolTable.hpp";

uint16_t SymbolTable::getSymbol(std::string name) {
    return this->variableTable[name];
}

void SymbolTable::setSymbol(std::string name, uint16_t value) {
    this->variableTable.emplace(name,value);
}

// include only once in compilation
#pragma once

#include <string>
#include <unordered_map>

#include "process.h"

// Q4
class ProcessMap {
public:
    // stores process in a map because it's capable of being fetched in O(1) time
    // given the name
    std::unordered_map<std::string, Process> table;

    //adds the process to the map
    void add(const Process& p);
};

// include only once in compilation
#pragma once

#include <iostream>
#include <vector>

#include "process.h"

// Q3: Stores N processes in a contiguous array 
// linear-time traversal (O(N)). Append is amortized O(1).
class ProcessTable {
public:
    // vector of processes, can be traversed linearly
    std::vector<Process> processes;

    // stores the process in the processes vector
    void add(const Process& p);
};

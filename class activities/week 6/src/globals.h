#pragma once

#include <iostream>
#include <string>
#include <format>
#include <cstdint>
#include <functional>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <unordered_map>
#include <queue>
#include <vector>
#include <tuple>

#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <shared_mutex>

enum SchedulerType {
    FCFS,
    RR
};

using SymbolTable = std::unordered_map<std::string, uint16_t>;
using LogEntry = std::tuple<std::string, uint8_t, std::string>;
using Logger = std::vector<LogEntry>;

struct Config {
    uint8_t numCpu;
    SchedulerType schedulerType;
    uint32_t quantumCycles;
    uint32_t batchProcessFreq;
    uint32_t minIns;
    uint32_t maxIns;
    uint32_t delaysPerExec;
};

inline Config config;
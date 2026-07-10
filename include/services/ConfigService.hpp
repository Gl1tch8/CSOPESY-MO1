#pragma once
#include <string>
#include <cstdint>
struct Config {
    // defaults match the PDF sample config; used when a config.txt key is
    // missing/misspelled so fields are never left uninitialized.
    std::string schedulingAlgo = "fcfs";
    uint32_t quantumCycles    = 5;
    uint32_t cpuCount         = 4;
    uint32_t batchProcessFreq = 1;
    uint32_t minIns           = 1000;
    uint32_t maxIns           = 2000;
    uint32_t delayPerSec      = 0;

    uint64_t maxOverallMem    = 16384; // bytes; total contiguous main-memory arena size
    uint32_t memPerFrame      = 16;    // bytes; unused by the first-fit contiguous allocator — parsed/stored only, for a future paging assignment
    uint64_t memPerProc       = 4096;  // bytes; fixed size every process requests
};

class ConfigService {
    public:
        void parseConfigFile();
        const Config getConfig() const;
    
    private:
        // clamp parsed values to valid ranges (called at the end of parseConfigFile)
        void validate();
        Config config;
};
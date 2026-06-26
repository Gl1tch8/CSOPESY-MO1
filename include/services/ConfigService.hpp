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
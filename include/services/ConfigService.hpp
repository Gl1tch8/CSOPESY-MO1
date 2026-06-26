#pragma once
#include <string>
#include <cstdint>
struct Config {
    std::string schedulingAlgo;
    uint64_t quantumCycles;
    uint64_t cpuCount;
    uint64_t batchProcessFreq;
    uint64_t mixIns;
    uint64_t maxIns;
    uint64_t delayPerSec;
};

class ConfigService {
    public:
        void parseConfigFile();
        const Config getConfig() const;
    
    private:
        Config config;
};
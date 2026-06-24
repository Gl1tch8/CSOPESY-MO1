#pragma once
#include <string>
#include <cstdint>
struct Config {
    std::string schedulingAlgo;
    uint32_t quantumCycles;
    uint32_t cpuCount;
    uint32_t batchProcessFreq;
    uint32_t mixIns;
    uint32_t maxIns;
    uint32_t delayPerSec;
};

class ConfigService {
    public:
        void parseConfigFile();
        const Config getConfig() const;
    
    private:
        Config config;
};
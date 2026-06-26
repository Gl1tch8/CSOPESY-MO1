#include "../../include/services/ConfigService.hpp"

#include <fstream>
#include <string>

void ConfigService::parseConfigFile() {
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        return;
    }

    std::string key;
    std::string value;

    while (file >> key >> value) {
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        if (key == "num-cpu") {
            config.cpuCount = static_cast<uint32_t>(std::stoul(value));
        } else if (key == "scheduler") {
            config.schedulingAlgo = value;
        } else if (key == "quantum-cycles") {
            config.quantumCycles = static_cast<uint32_t>(std::stoul(value));
        } else if (key == "batch-process-freq") {
            config.batchProcessFreq = static_cast<uint32_t>(std::stoul(value));
        } else if (key == "min-ins") {
            config.minIns = static_cast<uint32_t>(std::stoul(value));
        } else if (key == "max-ins") {
            config.maxIns = static_cast<uint32_t>(std::stoul(value));
        } else if (key == "delay-per-exec") {
            config.delayPerSec = static_cast<uint32_t>(std::stoul(value));
        }
    }
}

const Config ConfigService::getConfig() const {
    return config;
}
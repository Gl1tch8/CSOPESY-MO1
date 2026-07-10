#include "../../include/services/ConfigService.hpp"

#include <algorithm>
#include <fstream>
#include <string>

void ConfigService::parseConfigFile() {
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        return; // keep struct defaults
    }

    std::string key;
    std::string value;

    // parse a uint32 token, ignoring malformed values (keeps the current default)
    auto parseUint = [](const std::string& v, uint32_t fallback) -> uint32_t {
        try {
            return static_cast<uint32_t>(std::stoul(v));
        } catch (...) {
            return fallback;
        }
    };

    auto parseU64 = [](const std::string& v, uint64_t fallback) -> uint64_t {
        try {
            return static_cast<uint64_t>(std::stoull(v));
        } catch (...) {
            return fallback;
        }
    };

    while (file >> key >> value) {
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        if (key == "num-cpu") {
            config.cpuCount = parseUint(value, config.cpuCount);
        } else if (key == "scheduler") {
            config.schedulingAlgo = value;
        } else if (key == "quantum-cycles") {
            config.quantumCycles = parseUint(value, config.quantumCycles);
        } else if (key == "batch-process-freq") {
            config.batchProcessFreq = parseUint(value, config.batchProcessFreq);
        } else if (key == "min-ins") {
            config.minIns = parseUint(value, config.minIns);
        } else if (key == "max-ins") {
            config.maxIns = parseUint(value, config.maxIns);
        } else if (key == "delay-per-exec") {
            config.delayPerSec = parseUint(value, config.delayPerSec);
        } else if (key == "max-overall-mem") {
            config.maxOverallMem = parseU64(value, config.maxOverallMem);
        } else if (key == "mem-per-frame") {
            config.memPerFrame = parseUint(value, config.memPerFrame);
        } else if (key == "mem-per-proc") {
            config.memPerProc = parseU64(value, config.memPerProc);
        }
    }

    validate();
}

void ConfigService::validate() {
    // Clamp every field to a finite, in-range value so no consumer (initializeCores,
    // cpuReadyQueues.resize, the instruction generator) is ever handed a negative/huge
    // size. Generating more than this many instructions is not physically feasible, so
    // the spec's [1, 2^32] is capped to an int-safe, buildable ceiling.
    constexpr uint32_t MAX_INS = 1u << 20; // ~1,048,576

    if (config.schedulingAlgo != "fcfs" && config.schedulingAlgo != "rr") {
        config.schedulingAlgo = "fcfs";
    }

    config.cpuCount         = std::clamp<uint32_t>(config.cpuCount, 1, 128);
    config.quantumCycles    = std::max<uint32_t>(config.quantumCycles, 1);
    config.batchProcessFreq = std::max<uint32_t>(config.batchProcessFreq, 1);

    config.minIns = std::clamp<uint32_t>(config.minIns, 1, MAX_INS);
    config.maxIns = std::clamp<uint32_t>(config.maxIns, config.minIns, MAX_INS);

    config.maxOverallMem = std::max<uint64_t>(config.maxOverallMem, 1);
    config.memPerFrame   = std::max<uint32_t>(config.memPerFrame, 1);
    // A process that needs more memory than physically exists could never be
    // scheduled and would spin forever at the tail of its ready queue. Clamp
    // rather than let that happen silently.
    config.memPerProc = std::clamp<uint64_t>(config.memPerProc, 1, config.maxOverallMem);
}

const Config ConfigService::getConfig() const {
    return config;
}
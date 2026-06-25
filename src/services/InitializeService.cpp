#include "../../include/services/InitializeService.hpp"

#include "../../include/services/ConfigService.hpp"
#include "../../include/misc/SystemState.hpp"

#include <sstream>

InitializeService::InitializeService() : Service() {}

std::string InitializeService::executeFlags(std::string input) {
    if (input.find("initialize") != 0) {
        return "Error: initialize command not recognized.";
    }

    ConfigService configService;
    configService.parseConfigFile();
    Config config = configService.getConfig();

    if (config.cpuCount == 0) {
        return "Error: could not load config.txt or cpu count is zero.";
    }

    SystemState::getInstance().initializeCores(static_cast<int>(config.cpuCount));

    std::ostringstream output;
    output << "System initialized with " << config.cpuCount << " CPU cores using "
           << config.schedulingAlgo << " scheduling.";
    return output.str();
}
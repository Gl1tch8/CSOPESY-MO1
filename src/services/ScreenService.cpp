#include "../../include/services/ScreenService.hpp"
#include "../../include/misc/SystemState.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

ScreenService::ScreenService() : Service() {}

std::string ScreenService::executeFlags(std::string input) {
    std::stringstream ss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 2) {
        return "Error: screen command requires flags (e.g. screen -ls, screen -s <name>, screen -r <name>)";
    }

    std::string flag = tokens[1];

    if (flag == "-ls") {
        return listProcesses();
    } 
    else if (flag == "-s") {
        if (tokens.size() < 3) {
            return "Error: screen -s requires a process name (e.g. screen -s process_name)";
        }
        std::string name = tokens[2];
        return openSessionWindow(name);
    } 
        else if (flag == "-r") {
        if (tokens.size() < 3) {
            return "Error: screen -r requires a process name.";
        }
        std::string name = tokens[2];
        auto proc = SystemState::getInstance().getProcessByName(name);
        if (!proc) {
            return "Error: no process found with name " + name;
        }
        std::ostringstream out;
        out << "Screen session: " << name << "\n";
        out << "State: " << (proc->getIsFinished() ? "Finished" : "Running") << "\n";
        out << "Progress: " << proc->getCurrentLine() << " / " << proc->getTotalLines() << "\n";
        out << "--- Output ---\n";
        out << proc->getOutput();
        return out.str();
    }

    return "Error: Unrecognized screen flag '" + flag + "'";
}

std::string ScreenService::listProcesses() {
    SystemState &state = SystemState::getInstance();
    int totalCores = state.getNumCores();
    int usedCores = state.getActiveCores();
    int cpuUtilization = totalCores == 0 ? 0 : static_cast<int>((usedCores * 100) / totalCores);

    std::ostringstream output;
    output << "CPU utilization: " << cpuUtilization << "%\n";
    output << "Cores used: " << usedCores << "\n";
    output << "Cores available: " << (totalCores - usedCores) << "\n";
    output << "-----------------------------------------\n";
    output << "Running processes:\n";

    for (const auto& proc : state.getRunningProcesses()) {
        output << proc->getName() << "  (" << proc->getStartTimeStr() << ")  Core: "
               << proc->getCoreId() << "  " << proc->getCurrentLine() << " / "
               << proc->getTotalLines() << "\n";
    }

    output << "\nFinished processes:\n";
    for (const auto& proc : state.getFinishedProcesses()) {
        output << proc->getName() << "  (" << proc->getStartTimeStr() << ")  Finished  "
            << proc->getCurrentLine() << " / " << proc->getTotalLines() << "\n";
    }

    return output.str();
}

std::string ScreenService::newSessionProcess() {
    return "Error: screen -s requires a process name.";
}

std::string ScreenService::reattachSessionProcess() {
    return "Error: screen -r requires a process name.";
}

std::string ScreenService::openSessionWindow(std::string processName) {
    if (processName.empty()) {
        return "Error: process name is required.";
    }

    screens[processName] = nullptr;
    return "Opened screen session for " + processName;
}

std::string ScreenService::reportUtil() {
    std::string report = listProcesses();

    std::ofstream outFile("csopesy-log.txt");
    if (!outFile.is_open()) {
        return "Error: Could not open csopesy-log.txt for writing.";
    }

    outFile << report;
    return "Report generated at csopesy-log.txt!";
}

std::string ScreenMuxService::processSMI() {
    return "";
}
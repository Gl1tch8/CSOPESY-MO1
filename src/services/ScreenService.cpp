#include "../../include/services/ScreenService.hpp"
#include "../../include/misc/SystemState.hpp"
#include "../../include/misc/Helper.hpp"
#include "../../include/services/ConfigService.hpp"

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

        if (SystemState::getInstance().getProcessByName(name) != nullptr) {
            return "Error: screen with process name:" + name + " already exists";
        }

        ConfigService configService;
        configService.parseConfigFile();
        Config config = configService.getConfig();
        std::vector<Instruction> instrs = customInstructions(name, "instructions.txt", config.maxIns);
        
        int pid = static_cast<int>(SystemState::getInstance().getAllProcesses().size());
        ProcessInfo info;
        info.pid = pid;
        info.name = name;
        info.arrivalTime = SystemState::getInstance().getSystemTime();
        info.burstTime = 0;
        info.startTime = 0;
        info.endTime = 0;
        info.currentLineIndex = 0;
        info.state = ProcessState::NEW;
        info.instructions = instrs;
        info.totalLines = static_cast<int>(instrs.size());
        info.coreId = -1; // Unassigned initially

        Process process(info);
        SystemState::getInstance().addProcess(process);

        auto registeredProcess = SystemState::getInstance().getProcessByName(name);
        screens[name] = registeredProcess.get();
        activeScreen = name;
        return "";
    } 
        else if (flag == "-r") {
        if (tokens.size() < 3) {
            return "Error: screen -r requires a process name.";
        }
        return reattachSessionProcess(tokens[2]);
        // std::string name = tokens[2];
        // auto proc = SystemState::getInstance().getProcessByName(name);
        // if (!proc) {
        //     return "Error: no process found with name " + name;
        // }
        // std::ostringstream out;
        // out << "Screen session: " << name << "\n";
        // out << "State: " << (proc->getIsFinished() ? "Finished" : "Running") << "\n";
        // out << "Progress: " << proc->getCurrentLine() << " / " << proc->getTotalLines() << "\n";
        // out << "--- Output ---\n";
        // out << proc->getOutput();
        // return out.str();
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

// restore session
std::string ScreenService::reattachSessionProcess(std::string processName) {
    if (processName.empty()) {
        return "Error: screen -r requires a process name.";
    }
    auto proc = SystemState::getInstance().getProcessByName(processName);
    if (!proc || proc->getIsFinished()) {
        return "Process " + processName + " not found.";
    }
    screens[processName] = proc.get();
    activeScreen = processName;
    return "";
}

std::string ScreenService::openSessionWindow(std::string processName) {
    if (processName.empty()) {
        return "Error: process name is required.";
    }
    auto proc = SystemState::getInstance().getProcessByName(processName);
    if (!proc || proc->getIsFinished()) { // cant access finished processes
        return "Error: no process found with name " + processName;
    }
    
    screens[processName] = proc.get();
    activeScreen = processName;
    return ""; //screen loop is handled by main

    // return "Opened screen session for " + processName;
}
std::string ScreenService::getActiveScreen() const { return activeScreen; }

bool ScreenService::hasActiveScreen() const {
    return !activeScreen.empty();
}

void ScreenService::clearActiveScreen() {
    activeScreen.clear();
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

void ScreenService::addSessionLog(const std::string& processName, const std::string& log) {
    sessionLogs[processName].push_back(log);
}

std::vector<std::string> ScreenService::getSessionLogs(const std::string& processName) const {
    auto it = sessionLogs.find(processName);
    if (it != sessionLogs.end()) {
        return it->second;
    }
    return {};
}

std::string ScreenMuxService::processSMI(const std::string& processName) {
    auto proc = SystemState::getInstance().getProcessByName(processName);
    if (!proc) return "Error: process not found.";

    std::ostringstream out;
    out << "Process name: " << proc->getName() << "\n";
    out << "ID: " << proc->getPid() << "\n";
    out << "Logs:\n";
    out << proc->getOutput();

    if (proc->getIsFinished()) {
        out << "Finished!\n";
    } else {
        out << "\nCurrent instruction line: " << proc->getCurrentLine() << "\n";
        out << "Lines of code: " << proc->getTotalLines() << "\n";
    }

    return out.str();
}
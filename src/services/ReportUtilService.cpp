#include "../../include/services/ReportUtilService.hpp"
#include "../../include/misc/SystemState.hpp"
#include <fstream>
#include <sstream>

ReportUtilService::ReportUtilService() : Service() {}

std::string ReportUtilService::executeFlags(std::string input) {
    SystemState& state = SystemState::getInstance();

    std::stringstream ss;
    ss << "CPU utilization: " << static_cast<int>(state.getCpuUtilization()) << "%\n";
    ss << "Cores used: " << state.getCoresUsed() << "\n";
    ss << "Cores available: " << state.getCoresAvailable() << "\n";
    ss << "-----------------------------------------\n";

    ss << "Running processes:\n";
    for (const auto& proc : state.getRunningProcesses()) {
        ss << proc.getName() << "  (" << proc.getStartTimeStr() << ")  Core: " << proc.getCoreId()
           << "  " << proc.getCurrentLine() << " / " << proc.getTotalLines() << "\n";
    }
    ss << "\n";

    ss << "Finished processes:\n";
    for (const auto& proc : state.getFinishedProcesses()) {
        ss << proc.getName() << "  (" << proc.getStartTimeStr() << ")  Finished  "
           << proc.getCurrentLine() << " / " << proc.getTotalLines() << "\n";
    }

    std::string report = ss.str();

    std::ofstream outFile("csopesy-log.txt");
    if (!outFile.is_open()) {
        return "Error: Could not open csopesy-log.txt for writing.";
    }
    outFile << report;
    outFile.close();

    return "Report generated at csopesy-log.txt!";
}
#include "../../include/Process.hpp"

Process::Process(std::string name, int pid, std::string startTime, int coreId, int currentLine, int totalLines, bool isFinished)
    : name(name), pid(pid), startTime(startTime), coreId(coreId), currentLine(currentLine), totalLines(totalLines), isFinished(isFinished) {}

std::string Process::getName() const { return name; }
int Process::getPid() const { return pid; }
std::string Process::getStartTime() const { return startTime; }
int Process::getCoreId() const { return coreId; }
void Process::setCoreId(int id) { coreId = id; }
int Process::getCurrentLine() const { return currentLine; }
void Process::setCurrentLine(int line) { currentLine = line; }
int Process::getTotalLines() const { return totalLines; }
bool Process::getIsFinished() const { return isFinished; }
void Process::setIsFinished(bool finished) { isFinished = finished; }

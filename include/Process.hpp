#pragma once

#include <string>

class Process {
public:
    Process(std::string name, int pid, std::string startTime, int coreId, int currentLine, int totalLines, bool isFinished);

    std::string getName() const;
    int getPid() const;
    std::string getStartTime() const;
    int getCoreId() const;
    void setCoreId(int coreId);
    int getCurrentLine() const;
    void setCurrentLine(int line);
    int getTotalLines() const;
    bool getIsFinished() const;
    void setIsFinished(bool finished);

private:
    std::string name;
    int pid;
    std::string startTime;
    int coreId;
    int currentLine;
    int totalLines;
    bool isFinished;
};

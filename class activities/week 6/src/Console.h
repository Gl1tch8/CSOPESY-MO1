#pragma once

#include "globals.h"
#include "Process.h"

class Console {
    public:
        virtual ~Console() = default;

        virtual void run() = 0;
        virtual void setRunning(bool running) = 0;
        virtual void setProcess(std::shared_ptr<Process> process) {}
    private:
        virtual void executeCommand(const std::string& cmd) = 0;
};
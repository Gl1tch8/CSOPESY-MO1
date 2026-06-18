#pragma once

#include "globals.h"
#include "Console.h"
#include "Process.h"

class Screen : public Console {
    public:
        Screen(std::string name);
        ~Screen() = default;

        void run() override;
        void setRunning(bool running) override;
        void setProcess(std::shared_ptr<Process> process) override;

    private:
        void executeCommand(const std::string& cmd) override;
        void display();

        std::string name;
        bool running = false;
        std::shared_ptr<Process> process = nullptr;
};

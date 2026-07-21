#pragma once

#include "../../src/interfaces/Service.hpp"
#include <string>

class SchedulerService; // forward decl, mirrors ScreenService's pattern

class VmstatService : public Service {
public:
    VmstatService();

    // wire the scheduler used to read memory-manager stats
    void setScheduler(SchedulerService* s) { scheduler = s; }

    std::string executeFlags(std::string input) override;

private:
    SchedulerService* scheduler = nullptr;
};

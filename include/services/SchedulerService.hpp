#include "../../src/interfaces/Service.hpp"
#include <string>
#include <atomic>
class SchedulerService : public Service {
public:
    std::atomic<bool> generating = false;
    SchedulerService();

    std::string executeFlags(std::string input);

    void start();

    void stop();

    void run();
};
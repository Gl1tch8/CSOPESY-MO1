#include "../../src/interfaces/Service.hpp"
#include <string>

class SchedulerService : public Service {
public:

    SchedulerService();

    std::string executeFlags(std::string input);
};
#include "../interfaces/Service.hpp"
#include <string>
class SchedulerService : public Service {
    public: 

        SchedulerService():Service() {}

        std::string executeFlags(std::string input) {
            return input + " command recognized. Doing something.";
        }

};
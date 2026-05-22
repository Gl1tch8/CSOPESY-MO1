#include "../interfaces/Service.hpp"
#include <string>
class InitializeService : public Service {
    public: 

        InitializeService():Service() {}

        std::string executeFlags(std::string input) {
            return input + " command recognized. Doing something.";
        }

};
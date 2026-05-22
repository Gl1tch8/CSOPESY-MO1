#include "../interfaces/Service.hpp"
#include <string>
class ScreenService : public Service {
    public: 

        ScreenService():Service() {}

        std::string executeFlags(std::string input) {
            return input + " command recognized. Doing something.";
        }

};

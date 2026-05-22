#include "../interfaces/Service.hpp"
#include <string>

class ExitService : public Service {
    public: 

        ExitService():Service() {}

        std::string executeFlags(std::string input){
            return input + " command recognized. Doing something.";
        }

};
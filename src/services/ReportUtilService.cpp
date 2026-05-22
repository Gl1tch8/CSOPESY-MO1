#include "../interfaces/Service.hpp"
#include <string>
class ReportUtilService : public Service {
    public: 

        ReportUtilService():Service() {}

        std::string executeFlags(std::string input) {
            return input + " command recognized. Doing something.";
        }

};
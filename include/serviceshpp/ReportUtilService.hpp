#include "../../src/interfaces/Service.hpp"
#include <string>

class ReportUtilService : public Service {
public:

    ReportUtilService();

    std::string executeFlags(std::string input);
};
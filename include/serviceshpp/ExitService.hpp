#include "../../src/interfaces/Service.hpp"
#include <string>

class ExitService : public Service {
public:

    ExitService();

    std::string executeFlags(std::string input);
};
#include "../../src/interfaces/Service.hpp"
#include <string>

class InitializeService : public Service {
public:

    InitializeService();

    std::string executeFlags(std::string input);
};
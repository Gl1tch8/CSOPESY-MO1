#include "../../src/interfaces/Service.hpp"
#include <string>

class ScreenService : public Service {
public:

    ScreenService();

    std::string executeFlags(std::string input);
};
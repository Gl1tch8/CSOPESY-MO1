#pragma once

#include "../interfaces/Service.hpp"
#include <string>
class ClearService : public Service {
public:

    ClearService();

    std::string executeFlags(std::string input);

};
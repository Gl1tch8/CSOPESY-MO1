#include "../include/services/ClearService.hpp"

        ClearService::ClearService():Service() {}

        std::string ClearService::executeFlags(std::string input)  {
            return input + " command recognized. Doing something.";
        }

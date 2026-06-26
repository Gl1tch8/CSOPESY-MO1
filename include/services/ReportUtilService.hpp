#pragma once

#include "ScreenService.hpp"

class ReportUtilService : public ScreenService {
public:
    ReportUtilService();

    std::string executeFlags(std::string input);
};

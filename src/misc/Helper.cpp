#include "../../include/misc/Helper.hpp"


    std::string Helper::parse(std::string input) {
        // parse first word only and return that
        std::string command = input.substr(0, input.find(" "));
        return command;
    }


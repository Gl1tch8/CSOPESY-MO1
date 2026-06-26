#include "../../include/misc/Helper.hpp"
#include <chrono>
#include <ctime>

std::string Helper::getFormattedTime(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    char timeBuf[64];
    std::strftime(timeBuf, sizeof(timeBuf), format.c_str(), &tm);
    return std::string(timeBuf);
}


    std::string Helper::parse(std::string input) {
        // parse first word only and return that
        std::string command = input.substr(0, input.find(" "));
        return command;
    }


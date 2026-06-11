#pragma once

#include <chrono>
#include <ctime>
#include <string>

// Cross-platform "HH:MM:SS AM/PM" current-time string. The PDF uses the
// Windows-only localtime_s; here we branch to localtime_r on POSIX.
inline std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo{};
#if defined(_WIN32)
    localtime_s(&timeinfo, &t);
#else
    localtime_r(&t, &timeinfo);
#endif
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%I:%M:%S %p", &timeinfo);
    return std::string(buffer);
}

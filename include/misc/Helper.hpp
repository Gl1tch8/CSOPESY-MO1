#include <string>

class Helper {
public:
    std::string parse(std::string input);
    static std::string getFormattedTime(const std::string& format);
};
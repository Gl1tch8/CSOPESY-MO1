#include <string>

class Helper {
public:
    std::string parse(std::string input) {
        // parse first word only and return that
        std::string command = input.substr(0, input.find(" "));
        return command;
    }

};
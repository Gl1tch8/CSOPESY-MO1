#include "../../src/interfaces/Command.hpp"
#include "../../include/services/ScreenService.hpp"
#include <vector>

// libraries for sleep
#include <chrono>
#include <thread>
#include "../misc/Process.hpp"
class ScreenCommand : public Command {
public:
    // Accept the ScreenService pointer from main.cpp
    ScreenCommand(ScreenService* service);

    void print(std::string log);

    void execute(std::string input);

private:
    ScreenService* screenService;
    ScreenMuxService* screenMuxService;
};
#pragma once
#include<cstdint>
typedef uint64_t CPUTick;

class Process;

class Core {
public:
    Core() : id(-1) {};
    Core(int id):id(id) {};

    int getId() const;
    void setId(int id);
    bool isActive() const;
    void setActive(bool active);
    void setId(int id);
    void setActiveProcess(Process *p);

    
    void run();
private:
    int id = 0;
    bool active = false;
    Process *process = nullptr;
};

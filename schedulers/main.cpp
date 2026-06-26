
#include"process.hpp"
int main() {
    ProcessTable pTable;
    unsigned int timeQuantum = 3;
    pTable.addProcess(Process(1,  5, 0, "Process A"));
    pTable.addProcess(Process(2,  3, 1, "Process B"));
    pTable.addProcess(Process(3,  4, 2, "Process C"));

    ProcessScheduler scheduler(pTable.processTable);
    // Round Robin or SJF or FCFS
    // param 2 is preemptive bool
    // param 3 is time quantum for Round Robin
    scheduler.schedule("FCFS", true, timeQuantum);

}
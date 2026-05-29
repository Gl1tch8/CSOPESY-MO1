#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class instruction;

// 1. Process representation
class Process {
public:
	enum State {
		NEW,
		READY,
		RUNNING,
		WAITING,
		TERMINATED
	};

	int pid; //unique process id
	std::string name; //process name
	State state; //process state
	int priority; //priority level of process
	int remainingTime; //CPU total time needed for process
	int memSize; //memory size required by process
	std::vector<instruction> instructions; //instructions for the process
};

// 2. Instruction tracking
class instruction {
public:
	int current; //index of current instruction

	void advance();

	void reset();

	void finishInstruction();

	void jumpTo(int index);

	void printCurrentInstruction();

};

//3. Store N processes effectively
class ProcessQueue {
public:
	std::vector<Process> processes; //array of processes
	int size; //number of processes in the queue

	void addProcess(Process p);

	void removeProcess(int pid);

	Process* getNextProcess();

	void printQueue();

};

//4. O(1) retrieval of processes
class ProcessRetrieval {
private:
	std::vector<Process> processes;
	std::unordered_map<int, Process*> processMap; //map of pid to process pointer

public:
	void addProcess(Process p);

	Process& getByIndex(int index);

	Process* getByPID(int pid);
};
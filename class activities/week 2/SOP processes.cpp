
// Process.h
// 1. Process representation. These are the attributes of a process, and will be done in Process().
// The attributes are based on the screenshot provided, which showed what an output looks like and what info is needed to be printed;
// the attributes below are based on that.

#ifndef PROCESS_H //
#define PROCESS_H // or #pragma once
#include <string>
#include <vector>

class Process{
public: 

	// The state is tracked using an enum since they are predefined, and also to make it easier to track which state a process is in
	enum State {
		READY, //ready state is not shown
		RUNNING, //running state is after ready, and is shown in the output
		FINISHED, //afterwards, it moves to the finished states
	};
	
	// A process has the following attributes based on the info that would be printed in the OS emulator:
	int id; //id for unique process
	std::string name; //name of the process ('process_'+id)

	State state; //state of the process
	int core; //which core its using
	std::string datetime; //date in mmddyy format + military time w seconds

	// To make it easier to track instruction lines:
	int current; //current instruction line
	int total; //total instruction lines

};
#endif

/********************************/
// Instruction.h
// 2. Which instruction line is being executed? 
// Based on the information provided, the contents of the instruction line being executed seems to not be needed,
// so for this SW, only the line number is tracked.
// The instruction line is actually an attribute in the Process class, namely: current and total. 
// By using these two attributes, it makes it easier to determine which line is being executed, making this implementation simpler.

#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "Process.h"

class Instruction {
public: 

	// increment the current instruction
	void nextLine(Process& p);

	// print the current instruction being executed (in a specific format)
	void printCurrent(const Process& p);

	// done executing last instruction line (true when current == total)
	bool isDone(const Process& p);

	// this is also where changing of process state takes place
	void setState(Process& p, Process::State newState);

};
#endif

/********************************/
// ProcessStore.h
// 3. Store N processes effectively, should be traversable in linear time.
// In order to make it traversable in linear time, a queue can be used to store the processes. 
// Since it's not specified that we need to retrieve for this question, ProcessStore() does not have dequeueing functionality. 

#ifndef PROCESSSTORE_H
#define PROCESSSTORE_H
#include "Process.h"

class ProcessStore {
public:

	// stores a process in a queue
	void addProcess(const Process& p); // queue will be used to store most effectively and linearly

	// returns N, how many processes are stored in the queue
	int size() const;


private:
	// holds the processes
	std::vector<Process> processes; 
};
#endif

/********************************/
// ProcessGet.h
// 4. Store and retrieve in O(1).
// Similar to the previous question, this will be done using a queue. 
// But this time, aside from adding to the queue, retrieval is also needed. 
// So a function for dequeueing, as well as function to tell if empty, is added,
// Unlike ProcessStore() class that can be seen as the enqueueing of processes only, this class goes both ways.

#ifndef PROCESSGET_H
#define PROCESSGET_H
#include "Process.h"

class ProcessGet{
public:
	// store using a queue
	void addProcess(const Process& p);

	// retrieve frontmost process in order of the queue. The function returns a process.
	Process getProcess();

	// retrieve specifically the ith process. The function also returns a process.
	Process& get(int i);

	int size() const;

	// track if empty
	bool empty() const;
	

private:
	std::vector<Process> processes;
};
#endif
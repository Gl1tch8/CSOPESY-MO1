#pragma once
#include<string>
#include<iostream>
#include<vector>
#include<unordered_map>
#include <algorithm>
#include <thread>
#include <atomic>
class Process {
    public:
        unsigned int pId;
        unsigned int pBurstTime;
        unsigned int pArrivalTime;
        std::string pName;

        Process(unsigned int id,  unsigned int burstTime, unsigned int arrivalTime, std::string name) {
            pId = id;
            pBurstTime = burstTime;
            pArrivalTime = arrivalTime;
            pName = name;
        }

          void execute() {
            std::cout << "Executing process: "<< this->pName << "| ID:" << this->pId << "| Arrival Time:" << this->pArrivalTime << "| Burst Time:" << this->pBurstTime << std::endl;
            this->pBurstTime--;
        }
};



class ProcessTable {
public:
    std::unordered_map<unsigned int, Process> processTable;

    void addProcess(const Process& p) {
        processTable.insert_or_assign(p.pId, p);
    }

    Process& getProcess(unsigned int pId) {
        return processTable.at(pId);
    }
};

class ProcessScheduler {
    public:
        std::vector<Process> readyQueue;

        ProcessScheduler(std::unordered_map<unsigned int,Process> pTable) {
            for(auto& p: pTable) {
                readyQueue.push_back(p.second);
            }
        };

        void schedule(std::string algorithm, bool preemptive = true, unsigned int timeQuantum = 1) {
            std::atomic<uint> time = 0;
            bool sortedOnce = false;
            sortFCFS();
            while(this->readyQueue.size() > 0) {
                std::cout <<"Time: " << time << std::endl;

                if(algorithm == "SJF") {
                    if(preemptive) {
                        sortSJF(time);
                    } else {
                        if(!sortedOnce) {
                            sortSJF(time);
                            sortedOnce = true;
                        }
                    }
                } else if(algorithm == "Round Robin") {
                    if(!sortedOnce) {
                        sortFCFS();
                        sortedOnce = true;
                    }
                    sortRoundRobin(time,timeQuantum);
                }
                readyQueue[0].execute();
                if(readyQueue[0].pBurstTime == 0) {
                    readyQueue.erase(readyQueue.begin());
                }
                time++;
            }
        };

        // sort by arrival time for FCFS
        void sortFCFS() {
            std::sort(readyQueue.begin(),readyQueue.end(), [](Process a, Process b) {
                return a.pArrivalTime < b.pArrivalTime;
            });
        };
        // sort by burst time for SJF, but only for processes that have arrived by the current time
        void sortSJF(uint time) {
                std::sort(readyQueue.begin(),readyQueue.end(), [time](Process a, Process b) {
                    bool aReady = a.pArrivalTime <= time;
                    bool bReady = b.pArrivalTime <= time;
                    
                    if(aReady && bReady) {
                        return a.pBurstTime < b.pBurstTime;
                    } else if (aReady && !bReady) {
                        // If a is ready and b is not, a should come before b
                        return true;
                    } else if (!aReady && bReady) {
                        // If b is ready and a is not, b should come before a
                        return false;
                    }
                   else return false;
                }
            );
            
        };

        void sortRoundRobin(uint time, unsigned int tq) {
            if(time > 0 && time % tq == 0) {
                std::rotate(readyQueue.begin(), readyQueue.begin() + 1, readyQueue.end());
            }
        };



        
};
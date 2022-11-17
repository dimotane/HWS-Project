using namespace std;
#include <string>
#include <algorithm>  
#include "rbtree.h"

class Process {
    public:
        unsigned int pid, burst, arrival, priority, basePriority, lastRun, io;//process information
        string log; //empty string we can append to to log process history
        Process* next; //pointers for next and previous process in doubly linked list
        Process* prev;  
    Process(int x,int y,int z)//constructor used to create and then insert processes
    {
        pid = x;
        burst = y;
        priority = basePriority = z;
        next = prev = nullptr;
    }
};

class Queue {
    public:

    Process* headProcess;//last in
    Process* currentProcess;//first in
    int size;//for keeping track of processes in queue
    int priority;//for purposes of navigating queue tree when inserting process

    Queue(int p)//constructor used to create and then insert queues
    {
        this->priority = p;
        this->currentProcess = this->headProcess = NULL;
    }

    //method for adding a new process to the queue
    void add(Process* process);
    //method for re-queueing a process into a new priority queue
    void reQueue(Process* process, int desiredPriority);
    //method to remove process from the bottom of a queue on completion
    Process* complete();
    //method to "execute" processes by subtracting 1 from burst and checking if done
    Process* executeProcess(int clockTick, int tq);
    //simple method to see if there is a process in the queue
    bool hasProcess();
};

void Queue::add(Process* process){
    if (!this->hasProcess())
    {//if queue is empty, all we need to do is set its current process to the new one
        this->currentProcess = headProcess = process;
    }
    else//otherwise we need to change the head process and add the node to the DLL
    {
    process->next = this->headProcess;
    this->headProcess->prev = process;
    this->headProcess = process;
    }
    #ifdef DEBUG
    temp->log += "Added process " + process.pid + " to queue " + this->priority 
    #endif
}

//remove completed process and return a pointer to the completed process
Process* Queue::complete(){
    Process* temp = this->currentProcess;
    this->currentProcess->prev->next = nullptr;
    this->currentProcess = this->currentProcess->prev;
    #ifdef DEBUG
    temp->log += "process " + process->pid " completed, removing from queue \n"
    cout <<< process->log 
    #endif
    return temp;
}

bool Queue::hasProcess(){
    return this->currentProcess!=nullptr;
}

Process* Queue::executeProcess(int clockTick, int tq)
{
    this->currentProcess->lastRun = clockTick;
    this->currentProcess->burst -= tq;
    return (this->currentProcess->burst == 0 ? nullptr : this->currentProcess);
}

void reQueue(Process* process, Queue* queues[], int desiredPriority){
    process->prev->next = process->next;
    process->next->prev = process->prev;
    queues[desiredPriority]->add(process);
    #ifdef DEBUG
    process->log += "re-queued process " + process->pid + " to new priority " + desiredPriority + "\n";
    #endif
}

void promote(Process* process, Queue* queues[], unsigned int offset){
    bool kernel = (process->basePriority >=50 ? true : false);
    int newPriority = (kernel) ? max(process->priority+offset, (unsigned) 99) : max(process->priority+offset, (unsigned) 50);
    reQueue(process, queues, newPriority);
}

void demote(Process* process, Queue* queues[], unsigned int offset){
    int newPriority = max(process->basePriority, process->priority-offset);
    reQueue(process, queues, newPriority);
}

int* findAgedProcesses(Queue* queue)
{

}





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
        lastRun = 0;
    }
};

class Queue {
    public:

    Process* headProcess;//last in
    Process* currentProcess;//first in
    int size;//for keeping track of # of processes in queue

    Queue()//constructor used to instatiate default values in queue
    {
        currentProcess = headProcess = nullptr;
    }

    //methods for the queue class are as follows

    //method for adding a new process to the queue
    void add(Process* process);
    //method for re-queueing a process into a new priority queue
    void reQueue(Process* process, int desiredPriority);
    //method to remove process from the bottom of a queue on completion
    Process* complete();
    //method to "execute" processes by subtracting 1 from burst and checking if done
    int executeProcess(int clockTick, int tq);
    //method which returns a pointer to an array containing pointers of all processes
    //whose age > int age 
    int * findAgedProcesses(int old);
    //simple method to see if there is a process in the queue
    bool hasProcess();
};

void Queue::add(Process* process){
    if (size==0)
    {//if queue is empty, all we need to do is set its current process to the new one
        currentProcess = headProcess = process;
    }
    else if (size==1)
    {
        process->next = currentProcess;
        currentProcess->prev = process; 
        headProcess = process;
    }
    //otherwise we need to change the head process and add the node to the DLL
    else
    {
    process->next = headProcess;
    headProcess->prev = process;
    headProcess = process;
    }
    size++;
    #ifdef DEBUG
    #endif
}

//remove completed process and return a pointer to the completed process
Process* Queue::complete(){
    Process* temp = currentProcess;
    currentProcess->prev->next = nullptr;
    currentProcess = currentProcess->prev;
    #ifdef DEBUG
    #endif
    size--;
    return temp;
}

//self explanitory 
bool Queue::hasProcess(){
    return currentProcess!=nullptr;
}

//very simply decriments burst of process, logic for process completing/not completing
//is handled in the function that calls executeProcess
int Queue::executeProcess(int clockTick, int tq)
{
    currentProcess->lastRun = clockTick; //update lastrun of process
    currentProcess->burst -= tq;//subtract 
    return currentProcess->burst;//return remaining burst time. 
}

void reQueue(Process* process, Queue* queues[], int desiredPriority){
    process->prev->next = process->next;//link previous process to next process
    process->next->prev = process->prev;//vice versa 
    process->next = process->prev = nullptr; //reset pointers of process to null
    queues[process->priority]->size--;//decriment size of queue process is in by 1
    queues[desiredPriority]->add(process);//add old process to new queue
    #ifdef DEBUG//console output for debugging and logging
    #endif
}

//promote a given process by an offset
void promote(Process* process, Queue* queues[], unsigned int offset){
    bool kernel = (process->basePriority >=50 ? true : false);
    int newPriority = (kernel) ? max(process->priority+offset, (unsigned) 99) : max(process->priority+offset, (unsigned) 50);
    reQueue(process, queues, newPriority);
}

//demote a given process by an offset
void demote(Process* process, Queue* queues[], unsigned int offset){
    int newPriority = max(process->basePriority, process->priority-offset);
    reQueue(process, queues, newPriority);
}

//returns pointer to an array containing all processes whose age is greater than int old
int* Queue::findAgedProcesses(int old)
{

}





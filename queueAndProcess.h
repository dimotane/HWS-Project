#include <string>
#include <iostream>
#include <algorithm>  
#include <vector>
#include <iomanip>//this is for cool formatting

//to debug, uncomment the lines below as needed. these will significantly slow down the program

#define DEBUG //normal debuggind for process completion and rescheduling
#define DEBUG1 //additional level of debugging which shows processes arriving in queues
#define DEBUG2 //enables output for each burst, and printing of queues before/after a round robin
#define PRINT //enables printing of queue before and after processes move. insane slowdown

//Class defining a process, contains all its info
struct Process {
    int pid, burst, arrival, priority, basePriority, lastRun, io, completionTick, totalWait, numWaits;//process information
    bool kernel;
    Process* next; //pointers for next and previous process in doubly linked list
    Process* prev;

    Process(int a,int b,int c, int d, int e)//pid, burst, priority, arrival, io
    {
        pid = a;
        burst = b;
        priority = basePriority = c;
        arrival = lastRun = d;
        io = e; 
        next = prev = nullptr;
        kernel = basePriority >=50 ? true : false;
        totalWait =  numWaits = completionTick = 0;
    }

    Process()//default contstructor
    {
        pid = burst = priority = basePriority = arrival = lastRun = io = completionTick = totalWait = numWaits = 0;//make everything 0 lol
        next = prev = nullptr;
        kernel = false;
    }

    double calcAvgWait()//returns the average time the process spent waiting before seeing the CPU again
    {
        return totalWait/numWaits;
    }

    int calcTurnaround()//returns total time spent in scheduler
    {
        return completionTick-arrival;
    }
};

typedef Process* processPtr;//if i define this at the top vscode gets very upset


//Queue class, keeps track of first and last process in a linked list, as well as its size and priority
class Queue {
    public:
    processPtr lastProcess;//last in
    processPtr currentProcess;//first in
    int size;//for keeping track of # of processes in queue
    int priority;//for keeping track of active queues

    Queue(int p)//constructor used to instatiate default values in queue
    {
        currentProcess = lastProcess = nullptr;
        size = 0;
        priority = p;
    }

    //methods for the queue class are as follows

    //method for adding a new process to the queue
    bool add(processPtr process, int clockTick);
    //method for re-queueing a process into a new priority queue
    void reQueue(processPtr process,Queue queues[], int desiredPriority);
    //method to remove process from the bottom of a queue on completion
    bool complete(int clockTick, std::vector <Process> completedProcessList);
    //method to "execute" processes by subtracting 1 from burst and checking if done
    bool executeProcess(int clockTick);
    //prints the queue
    void printQueue();
};

std::string fixClockTick(int clockTick) {
    std::string out = "";
    std::string clockString = std::to_string(clockTick);
    int strLen = clockString.length();
    for (int i = 0; i < 8-strLen; i++) {
        out += "-";
    }
    out += std::to_string(clockTick);
    return out;
}

std::string fixQueue(int queue) {
return (std::to_string(queue).length()==1) ?  "0" + std::to_string(queue) : std::to_string(queue);
}


#ifdef PRINT
//i dont use this lol
void Queue::printQueue(){
    std::cout << "[PRINTING] Printing queue " << priority << " in order from current process to last process" << std::endl;
    processPtr proc = currentProcess;
    while (proc->prev != nullptr)
    {
        std::cout << "[PRINTING] PID: " << proc->pid << std::endl;
        proc = proc->prev;
    }
    std::cout << "[PRINTING] PID: " << proc->pid << std::endl;
}
#endif


//adds a process to queue, returns true if the queue was previously empty, false otherwise
bool Queue::add(processPtr process, int clockTick){
    if (size==0)
    {//if queue is empty, all we need to do is set its current process to the new one
         currentProcess = lastProcess = process;
    }
    else
    { //otherwise we need to change the last process and add the node to the DLL
    process->next = lastProcess;
    lastProcess->prev = process;
    lastProcess = process;
    }
    #ifdef PRINT
    printQueue();
    #endif
    size++;
    #ifdef DEBUG1//debugging messages
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(process->priority) << "] [PID:" << process->pid << "] added to queue" << std::endl;
    #endif
    return currentProcess->prev == nullptr; 
 }

//remove the current process from a given queue. returns true if it was the only process in queue.
bool Queue::complete(int clockTick, std::vector <Process> completedProcessList){
    completedProcessList.push_back(*currentProcess);//add completed process to completed process list
    currentProcess->completionTick = clockTick;
    bool solo = false;
    #ifdef DEBUG//debugging messages
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(priority) << priority << "] [PID:" << currentProcess->pid  << "] Completed" <<std::endl;
    #endif
    if (size > 1)
    {
        currentProcess->prev->next = nullptr;
        currentProcess = currentProcess->prev;
    } else
    {
        lastProcess = currentProcess = nullptr;
        solo = true;
    }
    size--;
    return solo;
}

//decriments burst by 1 and returns true only if burst reaches 0
bool Queue::executeProcess(int clockTick)
{
    if (!(currentProcess->lastRun = clockTick-1)){//log a wait time if the process hasn't run for at least 1 tick
        currentProcess->totalWait += clockTick - currentProcess->lastRun;//update the total wait time
        currentProcess->numWaits = (currentProcess->numWaits = currentProcess->arrival) ? currentProcess->numWaits = 0 : currentProcess->numWaits++; //AAAAAAAAAAAAAAAAAAAAA
    }
    currentProcess->lastRun = clockTick; //update lastrun of process
    currentProcess->burst -= 1;//subtract from the burst
    #ifdef DEBUG2
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(currentProcess->priority) << "] [PID:" << currentProcess->pid  << "] Executed one tick" <<std::endl;
    #endif
    return currentProcess->burst == 0;  //return true if process completes
}






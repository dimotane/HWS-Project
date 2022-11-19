#include <string>
#include <iostream>
#include <algorithm>  

#define DEBUG //normal debuggind for process completion and rescheduling
#define DEBUG1 //additional level of debugging which shows processes arriving in queues
//#define DEBUG2 //enables output for each burst, and printing of queues before/after a round robin

//Class defining a process, contains all its info
struct Process {
    int pid, burst, arrival, priority, basePriority, lastRun, io;//process information
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
    }
    Process() //default constructor for process
    {
        pid = 0;
        burst = 0;
        priority = basePriority = 0;
        arrival = 0;
        next = prev = nullptr;
        lastRun = 0;
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
    void reQueue(processPtr process,Queue* queues[], int desiredPriority);
    //method to remove process from the bottom of a queue on completion
    bool complete(int clockTick);
    //method to "execute" processes by subtracting 1 from burst and checking if done
    bool executeProcess(int clockTick);
    //method which returns a pointer to an array containing pointers of all processes
    //whose age > int age 
    int findAgedProcesses(int old);
    //simple method to see if there is a process in the queue
    void printQueue();
};

void Queue::printQueue(){
    std::cout << "Printing queue in order from current process to last process" << std::endl;
    processPtr proc = currentProcess;
    while (proc->prev != nullptr)
    {
        std::cout << "PID: " << proc->pid << std::endl;
        proc = proc->prev;
    }
    std::cout << "PID: " << proc->pid << std::endl;
}

//adds a process to queue, returns true if the queue was previously empty, false otherwise
bool Queue::add(processPtr process, int clockTick){
    if (size==0)
    {//if queue is empty, all we need to do is set its current process to the new one
        currentProcess = lastProcess = process;
    }
    else
    { //otherwise we need to change the head process and add the node to the DLL
    process->next = lastProcess;
    lastProcess->prev = process;
    lastProcess = process;
    }
    size++;
    #ifdef DEBUG1
    std::cout << "[" << clockTick << "][QUEUE:" << process->priority << "][PID:" << process->pid << "] added to queue" << std::endl;
    #endif
    return currentProcess->prev == nullptr; 
}

//remove the current process from a given queue. returns true if it was the only process in queue.
bool Queue::complete(int clockTick){
    bool solo = false;
    #ifdef DEBUG
    std::cout << "[" << clockTick << "][QUEUE:" << priority << "][PID:" << currentProcess->pid  << "] Completed" <<std::endl;
    #endif
    if (size > 1)
    {
        currentProcess->prev->next = nullptr;
        currentProcess = currentProcess->prev;
    }
    else
    {
        lastProcess = currentProcess = nullptr;
        solo = true;
    }
    size--;
    //std::cout << ". Queue is now of size " << size;
    return solo;
}

//decriments burst by 1 and returns true only if burst reaches 0
bool Queue::executeProcess(int clockTick)
{
    currentProcess->lastRun = clockTick; //update lastrun of process
    currentProcess->burst -= 1;//subtract 
    #ifdef DEBUG2
    std::cout << "Process PID:" << currentProcess->pid << " executed tick on clock tick " << clockTick << " , " <<currentProcess->burst <<" burst time remaining" << std::endl; 
    #endif
    return currentProcess->burst == 0;  //return true if process completes
}

//insert a given process
void reQueue(processPtr process, Queue* queues[], int desiredPriority, int clockTick){
    if (process->prev != nullptr){
    process->prev->next = process->next;//link previous process to next process
    }
    if (process->next != nullptr){
    process->next->prev = process->prev;//vice versa 
    }
    process->next = process->prev = nullptr; //reset pointers of process to null
    queues[process->priority]->size--;//decriment size of queue process is in by 1
    queues[desiredPriority]->add(process, clockTick);//add old process to new queue
    #ifdef DEBUG//console output for debugging and logging
    std::cout << "Procces PID:" << process->pid <<" re-queued to new queue " << desiredPriority; 
    #endif
}


//moves the first process in a queue to the end and updates queue accordingly 
void roundRobin(Queue* queue, int clockTick)
{
    #ifdef DEBUG
    std::cout << "[" << clockTick << "]" << "[QUEUE:" << queue->priority << "][PID:" << queue->currentProcess->pid << "] Round robin" << std::endl;
    #endif
    if (queue->size > 1)
    {
        processPtr newLast = queue->currentProcess;//create temp pointer to previous first process
        newLast->prev->next = nullptr;//set the next ponter of second in line to null
        queue->currentProcess = newLast->prev;//set current process to second in line
        queue->lastProcess->prev = newLast;//set prev pointer of old last process to new last process
        newLast->next = queue->lastProcess;//set next pointer of new last process 
        newLast->prev = nullptr;//new last in line should not have anything in prev pointer
        queue->lastProcess = newLast;//finally, update last process pointer in queue
    }
}

//promote a given process by 10
void promote(processPtr process, Queue* queues[], int clockTick){
    int newPriority = (process->kernel) ? std::min(process->priority+10, 99) : std::min(process->priority+10, 49);//big complicated statement that finds the new priority
    (newPriority == process->priority) ? roundRobin(queues[process->priority], clockTick) : reQueue(process, queues, newPriority, clockTick);//if new priority is the same as the old priority, roundrobin, else requeue
}

//demote a given process by an offset (time spent in cpu)
void demote(processPtr process, Queue* queues[], int offset, int clockTick){
    int newPriority = std::max(process->basePriority, process->priority-offset);//kernel/user need not apply since a process cannot deomte past its base priority. this broke the scheduler for like an entire day bc i was using unsigned ints lol 
    (newPriority == process->priority) ? roundRobin(queues[process->priority], clockTick) : reQueue(process, queues, newPriority, clockTick);//round robin if same priority, else requeue
}

//promotes all aged processes in a given queue and returns number of processes promoted
int Queue::findAgedProcesses(int old)
{
    return 0;
}





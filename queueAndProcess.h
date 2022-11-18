#include <string>
#include <algorithm>  

//Class defining a process, contains all its info
struct Process {
    unsigned int pid, burst, arrival, priority, basePriority, lastRun, io;//process information
    std::string log; //empty string we can append to to log process history
    Process* next; //pointers for next and previous process in doubly linked list
    Process* prev;

    Process(int a,int b,int c, int d, int e)//pid, burst, priority, arrival, io
    {
        pid = a;
        burst = b;
        priority = basePriority = c;
        arrival = d;
        io = e; 
        next = prev = nullptr;
        lastRun = 0;
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

typedef Process* processPtr;


//Class defining the first and last element in a linked list queue of processes
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
    bool add(processPtr process);
    //method for re-queueing a process into a new priority queue
    void reQueue(processPtr process,Queue* queues[], int desiredPriority);
    //method to remove process from the bottom of a queue on completion
    bool complete();
    //method to "execute" processes by subtracting 1 from burst and checking if done
    bool executeProcess(int clockTick);
    //method which returns a pointer to an array containing pointers of all processes
    //whose age > int age 
    int * findAgedProcesses(int old);
    //simple method to see if there is a process in the queue
    bool hasProcess();
};

bool Queue::add(processPtr process){
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
    #ifdef DEBUG
    #endif
    return currentProcess->prev == nullptr; 
}

//remove completed process and return a pointer to the completed process
bool Queue::complete(){
    if (currentProcess->prev != nullptr)
    {
        currentProcess->prev->next = nullptr;
        currentProcess = currentProcess->prev;
    }
    else
    {
        lastProcess = currentProcess = nullptr;
    }
    #ifdef DEBUG
    #endif
    size--;
    return currentProcess->prev == nullptr;
}

//self explanitory 
bool Queue::hasProcess(){
    return currentProcess!=nullptr;
}

//very simply decriments burst of process, logic for process completing/not completing
//is handled in the function that calls executeProcess
bool Queue::executeProcess(int clockTick)
{
    currentProcess->lastRun = clockTick; //update lastrun of process
    currentProcess->burst -= 1;//subtract 
    return currentProcess->burst == 0;  //return true if process completes
}

void reQueue(processPtr process, Queue* queues[], int desiredPriority){
    if (process->prev != nullptr){
    process->prev->next = process->next;//link previous process to next process
    }
    if (process->prev != nullptr){
    process->prev->next = process->prev;//vice versa 
    }
    process->next = process->prev = nullptr; //reset pointers of process to null
    queues[process->priority]->size--;//decriment size of queue process is in by 1
    queues[desiredPriority]->add(process);//add old process to new queue
    #ifdef DEBUG//console output for debugging and logging
    #endif
}

void roundRobin(Queue* queue)
{
    if (queue->size == 1 )
    {
        return;
    }
    else 
    {
        processPtr temp = queue->currentProcess;
        queue->currentProcess = temp->prev;
        temp->next = queue->lastProcess;
        queue->lastProcess = temp; 
    }
}

//promote a given process by an offset
void promote(processPtr process, Queue* queues[], unsigned int offset){
    bool kernel = (process->basePriority >=50 ? true : false);
    int newPriority = (kernel) ? std::max(process->priority+offset, (unsigned) 99) : std::max(process->priority+offset, (unsigned) 50);
    if (newPriority == process->priority)
    {
        roundRobin(queues[process->priority]);
    }
    else
    {
        reQueue(process, queues, newPriority);
    }
}

//demote a given process by an offset
void demote(processPtr process, Queue* queues[], unsigned int offset){
    int newPriority = std::max(process->basePriority, process->priority-offset);
    if (newPriority == process->priority)
    {
        roundRobin(queues[process->priority]);
    }
    else
    {
        reQueue(process, queues, newPriority);
    }
}

//returns pointer to an array containing all processes whose age is greater than int old
int* Queue::findAgedProcesses(int old)
{
    return 0;
}





using namespace std;
#include <string>

class Process{
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
        next = prev = NULL;
    }
};

class Queue {
    public:

    Process* headProcess;//last in
    Process* currentProcess;//first in
    //Queue* leftChild;
    //Queue* rightChild;
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
    Process* remove();
    //method to "execute" processes by subtracting 1 from burst and checking if done
    void executeProcess(int clockTick);
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
    temp->log += "Added process"
    #endif
}

void reQueue(Process* process,Queue* queues[], int desiredPriority){
    queues[desiredPriority]->add(process);
    #ifdef DEBUG
    temp->log += "re-queued process"
    #endif
}

//remove completed process and return a pointer to the completed process
Process* Queue::remove(){
    Process* temp = this->currentProcess;
    this->currentProcess->prev->next = NULL;
    this->currentProcess = this->currentProcess->prev;
    #ifdef DEBUG
    temp->log += "process completed, removing from queue"
    #endif
    return temp;
}

bool Queue::hasProcess(){
    return this->currentProcess!=NULL;
}

void Queue::executeProcess(int clockTick)
{
    this->currentProcess->lastRun = clockTick;
}

/*
//basic bitch binary search algorithm for our queue tree. 
//doesn't need to be in the queue class.
Queue* search(int desiredPriority, Queue* node){
    if (node->priority == desiredPriority)
    {
        return node;
    }
    else if (node->priority >= desiredPriority)
    {
        return search(desiredPriority,node->leftChild);
    }
    else if (node->priority <= desiredPriority)
    {
        return search(desiredPriority,node->rightChild);
    }
}
*/




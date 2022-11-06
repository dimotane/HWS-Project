using namespace std;
#include <string>

class Queue {
    public:
    Process* headProcess;
    Process* currentProcess;
    int size;
};

class Process{
    public:
        unsigned int pid, burst, arrival, priority, basePriority, deadline, waitTime, io;//process information
        string log; //empty string we can append to to log process history
        Process* next; //pointers for next and previous process in doubly linked list
        Process* prev;  
};

void addToQueue(Process* inProcess, Queue* queue)
{
    inProcess->next = queue->headProcess;
    inProcess->prev = NULL;
    queue->headProcess->prev = inProcess;
    queue->size++;
}

void removeFromQueue(Queue* queue)
{
    queue->currentProcess = queue->currentProcess->prev;
    queue->size--;
}

void roundRobin(Queue* queue)
{
    Process* temp = queue->currentProcess;
    queue->currentProcess = queue->currentProcess->prev;
    queue->headProcess->prev = temp;
    queue->headProcess = temp;
    free(temp);
}; 

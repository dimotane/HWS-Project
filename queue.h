using namespace std;
#include <string>
#include "process.h"

class Queue {
    public:
    Process* headProcess;
    Process* currentProcess;
    Queue* leftChild;
    Queue* rightChild;
    int size;
};

void addToQueue(Process* inProcess, Queue* queue)
{
    inProcess->next = queue->headProcess;
    inProcess->prev = NULL;
    queue->headProcess->prev = inProcess;
    queue->size++;
}

void reQueue()
{
    
}


void completeProcess(Queue* queue)
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
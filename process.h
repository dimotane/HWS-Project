using namespace std;
#include <string>
#include "queue.h"

class Process{
    public:
        unsigned int pid, burst, arrival, priority, basePriority, deadline, waitTime, io;//process information
        string log; //empty string we can append to to log process history
        Process* next; //pointers for next and previous process in doubly linked list
        Process* prev;  
    Process(int x,int y,int z)
    {
        pid = x;
        burst = y;
        priority = z;
    }
};

int executeProcess(Queue* queue, int tq)
{
    queue->currentProcess->burst -= tq;
    if (queue->currentProcess->burst <= 0)
    {
        completeProcess(queue); 
        return 1;
    }
    else 
    {
        roundRobin(queue);
        return 0;
    }
}

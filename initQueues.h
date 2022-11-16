using namespace std;
#include <string>
#include "queueAndProcess.h"

void initQueues(Queue* queues)
{
   
   for (int i = 0; i < 99; i++)
   {
    queues[i] = Queue(i);
   }
};
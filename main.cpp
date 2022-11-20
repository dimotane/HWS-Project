#include <set>
#include "framework.h"

static int promoteValue = 10;

//*******************************************************//
//            Mitchell Ewan & Jay Grzybowski             //
//               Hybrid windows scheduler                //
//                                                       //
// Takes user input, either in the form of an input file //
// full of processes or manual input from the user, and  //
//   simulates a hybrid windows scheduler, dynamically   //
// adjusting the priorities of the processes and running //
//                them until completion                  //
//*******************************************************//

//FOR DEBUGGING: see instructions at top of queueProcess.h

//returns the highest active queue in the given active queue tree, or -1 if there are no active queues
int findHighestPriorityQueue(int clockTick, set <int> activeQueues, int activeQueue) 
{
    int newActive = *activeQueues.rbegin(); //update the active queue
    #ifdef DEBUG
    if ((newActive != -1) && (newActive != activeQueue)){//debugging messages (dont print if active queue didn't change)
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(newActive) << "] Became highest priority queue" << endl; 
    }
    #endif
    return newActive;
}

//add a priority to the active queue tree
void addActiveQueue(int priority, int clockTick , set <int> & activeQueues){
    activeQueues.insert(priority);
    #ifdef DEBUG
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(priority) << "] Became Active" << std::endl;
    #endif
}

//remove a priority from the active queue tree
void removeActiveQueue(int priority, int clockTick, set <int> & activeQueues){
    activeQueues.erase(priority);
    #ifdef DEBUG
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(priority) << "] Became Inactive" << std::endl;
    #endif
    
}

//used by the vector sorting method to order our processes so lowest priority is at the back
bool sortProcessVector(Process p1, Process p2)
{
    int a1 = p1.arrival;
    int a2 = p2.arrival;
    if (a1 == a2 )
    {
        return p1.pid > p2.pid; 
    }
    return (a1>a2);
}

//inserts a given process to a new queue: returns 1 if a new queue became active, -1 if the old queue became inactive, or 0 if there are no changes to active queues
int reQueue(processPtr process, Queue * queues[], int desiredPriority, int clockTick){
    Queue * oldQueue = queues[process->priority];//create a pointer to the old queue for convenience 
    oldQueue->size--;//decriment size of queue process is in by 1

    #ifdef DEBUG2//debugging messages
    if (desiredPriority > process->priority){
        std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(process->priority) << "] [PID:" << process->pid << "] Promoting to new queue" << std::endl;
    }else
    {
        std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(process->priority) << "] [PID:" << process->pid << "] Demoting to new queue" << std::endl;
    }
    #endif

    int queueChanges = 0; //0 if no changes, -1 if a queue became inactive, 2 if a queue became active, 1 if both happened

    if (oldQueue->size == 0) {//see if old queue became inactive
    queueChanges -= 1;
    } else if (process->next == nullptr)//if requeuing process is current process
    {
        oldQueue->currentProcess = process->prev;
        process->prev->next = nullptr;
    } else if (process->prev == nullptr)//if requeuing process is last process
    {  
        oldQueue->lastProcess = process->next;
        process->next->prev = nullptr;
    } else //otherwise update the processes before and after this one
    {
        process->next->prev  = process->prev;
        process->prev->next = process->next;
    }

    process->priority = desiredPriority;//update priority of process
    process->next = process->prev = nullptr; //reset pointers of process to null

    if (queues[desiredPriority]->add(process, clockTick)){//add process to new queue, +2 if it was empty
        queueChanges+= 2;
    }

    return queueChanges; 
}

//moves the first process in a queue to the end and updates queue accordingly 
void roundRobin(Queue* queue, int clockTick)
{
    #ifdef DEBUG//debugging messages
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << queue->priority << "] [PID:" << queue->currentProcess->pid << "] Round robin" << std::endl;
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

//process movement driver: handles the logic and then moves a process
//determines if process needs to be round robin'd or re-queued, and updates the actiive queue list 
void moveProcess(processPtr process, Queue* queues[], int clockTick, int newPriority, set <int> * activeQueues) {
    int oldPriority = process->priority;//store the old priority in case a queue gets deactivated 
    if (newPriority == process->priority) {//round robin if same priority, else requeue
    roundRobin(queues[oldPriority], clockTick);
    } else //if requeued, see if new queue became active
    {
        int queueUpdate = (reQueue(process,queues,newPriority,clockTick)); 
        if (queueUpdate == 2 || queueUpdate == 1){
            addActiveQueue(newPriority, clockTick, *activeQueues);//update priority queue tree
        }
        if (queueUpdate == -1 || queueUpdate == 1)
        {
            removeActiveQueue(oldPriority, clockTick, *activeQueues);
        }
    } 
    process->lastMovement = clockTick;
}

//promote a given process by $promoteValue, 10 as of now
void promote(processPtr process, Queue* queues[], int clockTick, set <int> * activeQueues) {
    int newPriority = (process->kernel) ? std::min(process->priority+promoteValue, 99) : std::min(process->priority+promoteValue, 49);//big complicated statement that finds the new priority
    moveProcess(process, queues, clockTick, newPriority, activeQueues);
}

//demote a given process by an offset (time spent in cpu)
void demote(processPtr process, Queue* queues[], int offset, int clockTick, set <int> * activeQueues) {
    int newPriority = std::max(process->basePriority, process->priority-offset);//kernel/user need not apply since a process cannot deomte past its base priority.
    moveProcess(process, queues, clockTick, newPriority, activeQueues);
} 

//promote all aged processes by 10 
int promoteAgedProcesses(Queue * queues[], int age, int clockTick, int activeQueue, set <int> * activeQueues)
{
    int totalPromotions = 0;//will print
    bool isActive;//used to prevent promoting the currently running process 
    int oldEnough = clockTick - age; //if process last ran before this clock tick, promote it
    Process temp; //use these to keep track of the next process in queue if a process gets moved to a new queue
    processPtr tempProc = &temp;//make a pointer pointing to the temporary process

    //loop through all queues
    for (int i = 0; i <= 99; i++){
        isActive = (i == activeQueue);
        if (i == 49 || i == 99 || queues[i]->currentProcess == nullptr){//break if no current process, queue is empty. processess in 49 and 99 cannot be promoted so continue
            continue;
        }


        //make a pointer to the last process in queue
        processPtr iterator = queues[i]->lastProcess;

        //check if iterator is old enough to be promoted. 
        //if it is, temporarily store the process's values, promote it ,and iterate 
        do
        {   
            bool wasPromoted = false;
            if (iterator->lastRun < oldEnough && iterator->lastMovement < oldEnough){
                temp = *(iterator);//if process is getting promoted, load its data into the temp process
                promote(iterator, queues, clockTick, activeQueues);//promote the process
                wasPromoted = true;//process was promoted
                totalPromotions++;
            }
            //if the process was promoted we use a temporary pointer that stored its next process before it was promoted
            iterator = wasPromoted ? tempProc->next : iterator->next; 
        }
        while (iterator != nullptr);
    }
    #ifdef DEBUG
    if (totalPromotions > 0) {
    std::cout << "[" << fixClockTick(clockTick) << "] " << totalPromotions << " processes promoted" << endl;
    }
    #endif
    return 0; 
}

//===================================================================================================================================================================================================//
//===================================================================================================================================================================================================//
//===================================================================================================================================================================================================//

int main() {
    //set up vars
    int completedProcesses, totalProcesses, tq, age;
    int activeQueue = -1, clockTick = 1, cpuTime = 0;
    bool becameInactive = false;
    set <int> activeQueues;
    set <int> * setPtr = &activeQueues;
    activeQueues.insert(-1);
    Queue * queues[100];

    std::cout << "[?] Please enter the path to your input file, or type \"manual\" to manually enter processes. ";
    string inputpath; //"input.txt";
    std::cin >> inputpath;

    bool hasFile = (inputpath.compare("manual") != 0);
    vector<Process> inputProcessQueue = readFile(inputpath, hasFile); //read processes into a vector

    //exit if the file provided doesn't have any processes
    (inputProcessQueue.size() == 0) ? std::cout << "[-] No valid processes found. exiting." << endl : std::cout << "[-] Sorting " << inputProcessQueue.size() << " valid processes...";
    if (inputProcessQueue.size() == 0){
        return 1;
    }

    //sort based on arrival time
    sort(inputProcessQueue.begin(), inputProcessQueue.end(), sortProcessVector);
    std::cout << "Done!" << endl;

    //set up queues. i tried to make this its own method like 100 times but i can't lol 
    for (int i = 0; i <= 99; i++){
        queues[i] = new Queue(i);
        queues[i]->priority = i;
    }

    std::cout << "[?] Please enter time quantum: ";
    std::cin >> tq;
    std::cout << "[?] Please enter age required for promotion: ";
    std::cin >> age;

    //setup more vars to prepare to start scheduler 
    vector<Process> completedProcessList; //vector that will hold completed processes
    totalProcesses = inputProcessQueue.size();
    processPtr end = &inputProcessQueue.front();
    processPtr nextProcess = &inputProcessQueue.back();//pointer the nextProcess item in the vector (lowest arrival)
    processPtr tempLast = nextProcess; //used for debugging
    int lastInsertTick = end->arrival;//used to make sure we don't try to add a null process and fuck everything up

    std::cout << "[0] Starting Clock and Beginning HWS" << endl;
    while(completedProcesses < totalProcesses)//keep doing stuff until all expected processes are complete
    {   //process insertion loop
        while (nextProcess->arrival == clockTick && clockTick <= lastInsertTick) //insert processes whose arrival time matches current clock tick
        {
            if(queues[nextProcess->priority]->add(nextProcess, clockTick)) {//if the queue it got added to was empty, add that queue to the priority tree
                addActiveQueue(nextProcess->priority, clockTick, activeQueues);
            }
            inputProcessQueue.pop_back();
            tempLast = nextProcess; 
            nextProcess = &inputProcessQueue.back();
            if (activeQueue == -1) {
                activeQueue = findHighestPriorityQueue(clockTick, activeQueues, activeQueue);
            }
        }

        //check all queues for aged processes and promote them. this will not promote the active process
        if (clockTick >= age){
            promoteAgedProcesses(queues, age, clockTick, activeQueue, setPtr);
        }

        //this is the part that actually executes the process and stuff
        if (cpuTime < tq && activeQueue != -1) {//if current process has not used up its tq (and there is an active queue)
            if(queues[activeQueue]->executeProcess(clockTick)) {//execute one tick of current process and see if it completes
                if(queues[activeQueue]->complete(clockTick, completedProcessList)) {//if it completes, see if it was the only process in queue
                    removeActiveQueue(activeQueue, clockTick, activeQueues);//if it was the only process, remove the queue from active
                }
                activeQueue = findHighestPriorityQueue(clockTick, activeQueues, activeQueue);//update highest priority queue
                completedProcesses++;//incriment processes completed
                cpuTime = -1;//reset cpu time for next process
            }
        }else if (activeQueue != -1)//if the process has used up its time quantum, demote it
        {   
            #ifdef DEBUG//output messages
            std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(activeQueue) << "] [PID:" << queues[activeQueue]->currentProcess->pid << "]" << " Time quantum expired, demoting" << std::endl; 
            #endif
            demote(queues[activeQueue]->currentProcess, queues, cpuTime, clockTick, setPtr);//demote the process by amount of time it has spent in the cpu
            activeQueue = findHighestPriorityQueue(clockTick, activeQueues, activeQueue);
            cpuTime = -1;//set to -1 to reset to 0 once cpuTime++ occurs
            clockTick--;//clock should not incriment during demotion
        }
        cpuTime++; //increment cpu time and clock tick, reset inactive bool, this clock tick is complete
        clockTick++;
        becameInactive = false;
    }

    //init values used for stats
    double totalWaitTime, totalTurnaroundTime, avgWaitTime, avgTurnaroundTime;
    
    //prepare to gather stats from completed processes
    processPtr proc = &completedProcessList.back();

    //calculate the stats
    while (completedProcessList.size() > 1)
    {  
        totalWaitTime += proc->calcAvgWait();
        totalTurnaroundTime += proc->calcTurnaround();
        completedProcessList.pop_back();
        proc = &completedProcessList.back();
    }
    avgWaitTime = totalWaitTime / totalProcesses;
    avgTurnaroundTime = totalTurnaroundTime / totalProcesses;

    //fixes the formatting on stats to make them have only two decimal places
    std::cout.unsetf (std::ios::floatfield);
    std::cout.precision(3);

    //print the stats and exit
    std::cout << "[-] All processes complete: HWS Simulation complete. Simulated " << totalProcesses << " processes in " << clockTick-1 << " clock ticks." << endl;
    std::cout << "[-] Average wait time: " << avgWaitTime << " ticks" << endl;
    std::cout << "[-] Average turnaround time: " << avgTurnaroundTime << " ticks" << endl;
    return 0;
};







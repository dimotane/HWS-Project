#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include "queueAndProcess.h"
#include "redBlackTree.h"

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

//this function reads the input file and returns a vector of all the valid processes in it
//alternitively, if hasFile is false, the user can input processes manually
vector <Process> readFile(string filepath, bool hasFile){
    vector <Process> pVector;
    ifstream InputFile(filepath); //read in the input file
    array <string, 6> pieces{ "","","","","",""};
    string inputline; //input string
    char tab = '\t';
    int counter;
    if (hasFile) {
        std::cout << "[-] Reading input file...";
        getline(InputFile, inputline);//get rid of header line
        getline(InputFile, inputline);//get the first line
        do //using do while here because otherwise it skips the nextProcess line
        {
             //for the rest of the lines in the file, we do stuff to it:
                stringstream stream(inputline);//read in from current line
                counter = 0;
                while(!stream.eof()) {
                    getline(stream, pieces[counter], tab);
                    counter++;
                } //this while loop parses 1 process into 6 strings

                int pid = stoi(pieces[0]);
                int burst = stoi(pieces[1]);
                int arrival = stoi(pieces[2]);
                int priority = stoi(pieces[3]);
                int deadline = stoi(pieces[4]);
                int io = stoi(pieces[5]); //parse strings as ints 

                if (burst<1 || burst > 100 || priority <0 || priority>99 || io > burst || arrival < 1 || pid < 1) {
                    continue; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
                } else
                {   //create process with data gathered
                    Process process = Process(pid, burst, priority, arrival, io);
                    pVector.push_back(process); //add process to the end of the vector
                } 
        
        } while(getline(InputFile, inputline));
       //that's everything for the file
        std::cout << "Done!" << endl;
    }else
    {
    bool exit = false;
    bool first = true;
        string response;
        while(!exit){
            if (!first) {
            std::cout << endl << "[?] Would you like to enter another process?" << endl << "[-] Type \"yes\" to enter a process, or type \"no\" to continue. ";
            std::cin >> response;
            }
            if((response.compare("Yes") == 0 || response.compare("yes") == 0 || response.compare("YES") == 0) || first) {
                //do the intake code
                std::cout << "[-] Please enter process ID, burst, arrival, priority, deadline, and I/O separated by spaces: ";
                std::cin >> inputline;
                counter = 0;
                istringstream iss(inputline); //read the input string
                while(!iss.eof()){
                getline(iss, pieces[counter]);
                counter++;
            } //this while loop parses 1 process into 6 strings

            int pid = stoi(pieces[0]);
            int burst = stoi(pieces[1]);
            int arrival = stoi(pieces[2]);
            int priority = stoi(pieces[3]);
            int deadline = stoi(pieces[4]);
            int io = stoi(pieces[5]); //parse strings as ints 

            if (burst<1 || burst > 100 || priority <0 || priority>99 || io > burst || arrival < 1 || pid < 1 ){
                std::cout << endl << "[X] This process is invalid."; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
            } else {
            Process process = Process(pid, burst, priority, arrival, io);
            pVector.push_back(process); //add process to the end of the vector
            }
            } else if (response.compare("No") == 0 || response.compare("no") == 0 || response.compare("NO") == 0){
                exit = true; //continue
            } else {
                std::cout << endl << "[-] Please only type \"yes\" or \"no\". ";
            }
        }
    }
    return pVector;
}

bool sortProcessVector(Process p1, Process p2) //used by the vector sorting method to order our processes so lowest priority is at the back
{
    int a1 = p1.arrival;
    int a2 = p2.arrival;
    if (a1 == a2 )
    {
        return p1.pid > p2.pid; 
    }
    return (a1>a2);
}

//promote all processes in the queue 
void promoteAgedProcesses(Queue * queues[], int age, int clockTick, int activeQueue)
{
    int totalPromotions = 0;//will print
    bool isActive;//
    int oldEnough = clockTick - age; //if process last ran before this, promote it

    //don't do anything until there are actually processes in the queues
    if (activeQueue == -1){
        return;
    }

    //loop through all queues
    for (int i = 0; i <= 99; i++){
        isActive = (i == activeQueue);
        if (i == 49 || i == 99 || queues[i]->currentProcess == nullptr){//break if no current process, queue is empty. processess in 49 and 99 cannot be promoted so continue
            continue;
        }
        
        processPtr iterator = queues[activeQueue]->currentProcess;

        while (iterator->next != nullptr)
        {

            #ifdef DEBUG2//debugging messages
            #endif
        }
        
    }

    #ifdef DEBUG
    if (totalPromotions > 0){
    std::cout << "[" << fixClockTick(clockTick) << "]" << totalPromotions << " processes promoted" << endl;
    }
    #endif
}

//inserts a given process to a new queue (NOT DONE)
void reQueue(processPtr process, Queue * queues[], int desiredPriority, int clockTick){
    processPtr ref = process; 
    if (process->prev != nullptr){
    process->prev->next = process->next;//link previous process to next process
    }
    if (process->next != nullptr){
    process->next->prev = process->prev;//vice versa 
    }
    process->next = process->prev = nullptr; //reset pointers of process to null
    queues[process->priority]->size--;//decriment size of queue process is in by 1
    queues[desiredPriority]->add(process, clockTick);//add process to new queue
    #ifdef DEBUG//debugging messages
    std::cout << "Procces PID:" << process->pid <<" re-queued to new queue " << desiredPriority; 
    #endif
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

int updateActiveQueue(int clockTick, RBTree activeTree, Node* root, int activeQueue)//sets activequeue to highest active queue, or -1 if there are no active queues
{
    int newActive = (activeTree.maximum(root) == NULL) ? -1 : (activeTree.maximum(root)->priority);
    #ifdef DEBUG
    if ((newActive != -1) && (newActive != activeQueue)){
    std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << newActive << "] Became highest priority queue" << endl; \
    }
    #endif
    return newActive;
}

int main() {
    //set up vars
    int completedProcesses, totalProcesses, tq, age;
    int activeQueue = -1, clockTick = 1, cpuTime = 0;
    bool becameInactive = false; 
    RBTree activeTree;
    Node* root;
    Queue * queues[100];

    std::cout << "[?] Please enter the path to your input file, or type \"manual\" to manually enter processes. ";
    string inputpath; //"input.txt";
    std::cin >> inputpath;

    bool hasFile = (inputpath.compare("manual") != 0);
    vector<Process> inputProcessQueue = readFile(inputpath, hasFile); //read processes into a vector

    //exit if the file provided doesn't have any processes
    (inputProcessQueue.size() == 0) ? std::cout << "[-] No valid processes found. exiting." << endl : std::cout << "[-] Sorting " << inputProcessQueue.size() << " valid processes...";
    if (inputProcessQueue.size() == 0)
    {
        return 1;
    }

    //sort based on arrival time
    sort(inputProcessQueue.begin(), inputProcessQueue.end(), sortProcessVector);
    std::cout << "Done!" << endl;

    //set up queues. i tried to make this its own method like 100 times but i can't lol 
    for (int i = 0; i <= 99; i++)
    {
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
                activeTree.insert(nextProcess->priority);
                root = activeTree.getRoot();//update root of priority tree
                #ifdef DEBUG
                std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(nextProcess->priority) << "] Became Active" << std::endl;
                #endif
            }
            inputProcessQueue.pop_back();
            tempLast = nextProcess; 
            nextProcess = &inputProcessQueue.back();
            if (activeQueue == -1) {
                activeQueue = updateActiveQueue(clockTick, activeTree, root, activeQueue);
            }
        }

        //check all queues for aged processes and promote them. this will not promote the active process
        promoteAgedProcesses(queues, age, clockTick, activeQueue);

        //this is the part that actually executes the process and stuff
        if (cpuTime != tq && activeQueue != -1) {//if current process has not used up its tq
            if(queues[activeQueue]->executeProcess(clockTick)) {//execute one tick of current process and see if it completes
                if(queues[activeQueue]->complete(clockTick, completedProcessList)) {//if it completes, see if it was the only process in queue
                    activeTree.deleteNode(activeQueue);//if it was alone in queue, remove the priority from the active tree
                    root = activeTree.getRoot();//update root of active tree
                    becameInactive = true;//this flag is used for output messages
                }
                #ifdef DEBUG//debugging output
                if (becameInactive) {
                std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(activeQueue) << "] Became Inactive" << std::endl;
                }
                #endif
                activeQueue = updateActiveQueue(clockTick, activeTree, root, activeQueue);//update highest priority queue
                completedProcesses++;//incriment processes completed
                cpuTime = -1;//reset cpu time for next process
            }
        }else if (activeQueue != -1)//if the process has used up its time quantum, demote it
        {   
            #ifdef DEBUG//output messages
            std::cout << "[" << fixClockTick(clockTick) << "] [QUEUE:" << fixQueue(activeQueue) << "] [PID:" << queues[activeQueue]->currentProcess->pid << "]" << " Time quantum expired, demoting" << std::endl; 
            #endif
            demote(queues[activeQueue]->currentProcess, queues, cpuTime, clockTick);//demote the process by amount of time it has spent in the cpu
            activeQueue = updateActiveQueue(clockTick, activeTree, root, activeQueue);
            cpuTime = -1;//set to -1 to reset to 0 once cpuTime++ occurs
            clockTick--;//clock should not incriment during demotion
        }
        cpuTime++; //increment cpu time and clock tick, reset inactive bool, this clock tick is complete
        clockTick++;
        becameInactive = false;
    }

    double totalWaitTime, totalTurnaroundTime, avgWaitTime, avgTurnaroundTime;
    
    processPtr proc = &completedProcessList.back();

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
    std::cout << "[-] No active queues remain: HWS Simulation complete. Simulated " << totalProcesses << " processes in " << clockTick-1 << " clock ticks." << endl;
    std::cout << "[-] Average wait time: " << avgWaitTime << " ticks" << endl;
    std::cout << "[-] Average turnaround time: " << avgTurnaroundTime << " ticks" << endl;
    return 0;
};



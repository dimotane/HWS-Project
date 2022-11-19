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
vector<Process> readFile(string filepath, bool hasFile){
    vector<Process> pVector;
    ifstream InputFile(filepath); //read in the input file
    array<string, 6> pieces{ "","","","","",""};
    string inputline; //input string
    char tab = '\t';
    int counter;
    if (hasFile) {
        std::cout << "[-]Reading input file...";
        getline(InputFile, inputline);//get rid of header line
        getline(InputFile, inputline);//get the first line
        do //using do while here because otherwise it skips the last line
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
            if (!first)
            {
            std::cout << "\n[-]Would you like to enter another process?\n type \"yes\" to enter a process, or type \"no\" to continue. ";
            std::cin >> response;
            }
            if((response.compare("Yes") == 0 || response.compare("yes") == 0 || response.compare("YES") == 0) || first){
                //do the intake code
                std::cout << "[-]Please enter process ID, burst, arrival, priority, deadline, and I/O separated by spaces: ";
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

            if (burst<1 || burst > 100 || priority <0 || priority>99 || io > burst || arrival < 1 || pid < 1){
                std::cout << "\n[-]This process is invalid."; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
            }else{
            Process process = Process(pid, burst, priority, arrival, io);
            pVector.push_back(process); //add process to the end of the vector
            }
            }else if (response.compare("No") == 0 || response.compare("no") == 0 || response.compare("NO") == 0){
                exit = true; //continue
            }else{
                std::cout << "\nPlease only type \"yes\" or \"no\". ";
            }
        }
    }
    return pVector;
}

bool sortProcessVector(Process p1, Process p2) //used by the vector sorting method to order our processes so lowest priority is at the back
{
    int a1 = p1.arrival;
    int a2 = p2.arrival;
    return (a1>a2);
}

//EVIL global variables that I will try to localize in the future. or not =3
int clockTick = 1;
int highest;
int cputime = 0; 
bool becameInactive = false; 
RBTree activeTree; 
Node* root;
RBTree* treeAddr = &activeTree;
typedef Process* processPtr;

int updateActiveQueue(int clockTick)//sets activequeue to highest active queue, or -1 if there are no active queues
{
    int newActive = (activeTree.maximum(root) == NULL) ? -1 : (activeTree.maximum(root)->priority);
    if (newActive != -1){
    cout << "[" << clockTick << "][QUEUE:" << newActive << "] Became new active queue" << endl; \
    }
    return newActive;
}


void promoteAgedProcesses(Queue queues[])
{
    
}

int main() {

    //set up local vars
    int completedProcesses, totalProcesses, tq;
    int activeQueue = -1;
    Queue * queues[100];

    std::cout << "[-]Please enter the path to your input file, or type \"manual\" to manually enter processes.";
    string inputpath; //"input.txt";
    std::cin >> inputpath;

    bool hasFile = (inputpath.compare("manual") != 0);

    vector<Process> inputProcessQueue = readFile(inputpath, hasFile); //read processes into a vector
    
    (inputProcessQueue.size() == 0) ? std::cout << "[-]No valid processes found. exiting." << endl : std::cout << "[-]Sorting " << inputProcessQueue.size() << " valid processes...";

    if (inputProcessQueue.size() == 0)
    {
        return 1;
    }

    sort(inputProcessQueue.begin(), inputProcessQueue.end(), sortProcessVector); //sort based on arrival time
    std::cout << "Done!" << endl;

    //set up queues
    for (int i = 0; i <= 99; i++)
    {//set up all the queues 
        queues[i] = new Queue(i);
        queues[i]->priority = i;
    }

    std::cout << "[-]Please enter time quantum:";
    std::cin >> tq;

    //object used for inserting processes
    Process p;
    totalProcesses = inputProcessQueue.size();
    
    processPtr end = &inputProcessQueue.front();
    processPtr last = &inputProcessQueue.back();//pointer the last item in the vector (highest priority)
    processPtr tempLast = last; //used for debugging

    int insertedProcesses = 0;//used to make sure we don't try to insert a null pointer and fuck everything up
    int lastInsertTick = end->arrival;

    std::cout << "[0]Starting Clock and Beginning HWS" << endl;
    while(completedProcesses < totalProcesses)//keep doing stuff until all expected processes are complete
    {   //
        tempLast = last;//i use this for debugging
        while (last->arrival == clockTick && clockTick <= lastInsertTick) {//insert processes whose arrival time matches current clock tick
            if(queues[last->priority]->add(last, clockTick)) {//if the queue it got added to was empty, add that queue to the priority tree
                activeTree.insert(last->priority);
                root = activeTree.getRoot();//update root of priority tree
                cout << "[" << clockTick << "]" << "[QUEUE:" << last->priority << "] Became Active" << std::endl;
            }
            insertedProcesses++;
            inputProcessQueue.pop_back();
            tempLast = last; 
            last = &inputProcessQueue.back();
            if (activeQueue == -1) {
                activeQueue = updateActiveQueue(clockTick);
            }
        }


        //this is the part that actually executes the process and stuff
        if (cputime != tq && activeQueue != -1) {//if current process has not used up its tq
            if(queues[activeQueue]->executeProcess(clockTick)) {//execute one tick of current process and see if it completes
                if(queues[activeQueue]->complete(clockTick)) {//if it completes, see if it was the only process in queue
                    activeTree.deleteNode(activeQueue);//if it was alone in queue, remove the priority from the active tree
                    root = activeTree.getRoot();//update root of active tree
                    becameInactive = true;//this flag is used for output messages
                }
                #ifdef DEBUG//debugging output
                if (becameInactive){
                cout << "[" << clockTick << "]" << "[QUEUE:" << activeQueue << "] Became Inactive" << std::endl;
                }
                #endif
                activeQueue = updateActiveQueue(clockTick);//update highest priority queue
                completedProcesses++;//incriment processes completed
                cputime = -1;//reset cpu time for next process
            }
        }else if (activeQueue != -1)//if the process has used up its time quantum, demote it
        {   
            #ifdef DEBUG//output messages
            cout << "[" << clockTick << "]" << "[QUEUE:" << activeQueue << "][PID:" << queues[activeQueue]->currentProcess->pid << "]" << " Time quantum expired, demoting" << std::endl; 
            #endif
            demote(queues[activeQueue]->currentProcess, queues, cputime, clockTick);//demote the process by amount of time it has spent in the cpu
            activeQueue = updateActiveQueue(clockTick);
            cputime = -1;//set to -1 to reset to 0 once cputime++ occurs
            clockTick--;//clock should not incriment during demotion
        }
        cputime++; //increment cpu time and clock tick 
        clockTick++;
        becameInactive = false;
    }

    int avgWaitTime, avgTurnaroundTime;

    std::cout << "[*]No active queues remain: HWS Simulation complete. Simulated " << totalProcesses << " processes using " << clockTick-1 << " clock ticks." << endl;
    /*
    std::cout << "==========SIMULATION STATS============" <<endl;
    std::cout << "          stuff              " <<endl;
    std::cout << "          stuff              " <<endl;
    std::cout << "          stuff              " <<endl;
    std::cout << "          stuff              " <<endl;
    std::cout << "          stuff              " <<endl;
    */
    return 0;
};



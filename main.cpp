#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include "queueAndProcess.h"
#include "redBlackTree.h"

//#define DEBUG

//this function reads the input file and parses it into processes
vector<Process> readFile(string filepath){
    vector<Process> pVector;
    ifstream InputFile(filepath); //read in the input file
    array<string, 6> pieces{ "","","","","",""};
    string inputline; //input string
    char tab = '\t';
    int counter;

   getline(InputFile, inputline);//get rid of header line
   while(getline(InputFile, inputline)){ //for the rest of the lines in the file, we do stuff to it:
        stringstream stream(inputline);//read in from current line
        counter = 0;
        while(!stream.eof()){
            getline(stream, pieces[counter], tab);
            counter++;
        } //this while loop parses 1 process into 6 strings

        int pid = stoi(pieces[0]);
        int burst = stoi(pieces[1]);
        int arrival = stoi(pieces[2]);
        int priority = stoi(pieces[3]);
        int deadline = stoi(pieces[4]);
        int io = stoi(pieces[5]); //parse strings as ints 

        if (burst<1 || burst > 100 || priority <0 || priority>99 || io > burst || arrival < 1 || pid < 1){
            continue; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
        }
        else
        {   //create process with data gathered
            Process process = Process(pid, burst, priority, arrival, io);
            pVector.push_back(process); //add process to the end of the vector
        } 
   }//that's everything for the file
   bool exit = false;
    string response;
    while(!exit){
        cout << "\nWould you like to manually enter another process?\n type \"yes\" to enter a process, or type \"no\" to continue. ";
        cin >> response;
        if(response.compare("Yes") == 0 || response.compare("yes") == 0 || response.compare("YES") == 0){
            //do the intake code
            cout << "Please enter process ID, burst, arrival, priority, deadline, and I/O separated by spaces: ";
            cin >> inputline;
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
            cout << "\nThis process is invalid."; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
        }else{
        Process process = Process(pid, burst, priority, arrival, io);
        pVector.push_back(process); //add process to the end of the vector
        }
        }else if (response.compare("No") == 0 || response.compare("no") == 0 || response.compare("NO") == 0){
            exit = true; //continue
        }else{
            cout << "\nPlease only type \"yes\" or \"no\". ";
        }
    }
    return pVector;
}

bool processSort(Process p1, Process p2) //check if one is bigger than the other
{
    int a1 = p1.arrival;
    int a2 = p2.arrival;
    return (a1>a2);
}

//vars used in main
unsigned int clockTick = 1;
unsigned int tq;
unsigned int totalProcesses; 
unsigned int highest;
int activeQueue = -1;
int cputime = 0; 
bool becameInactive = false; 
RBTree activeTree; 
Node* root;
RBTree* treeAddr = &activeTree;
Queue * queues[100];
typedef Process* processPtr;

void updateActive()//sets activequeue to highest active queue, or null if there are no queues
{
    activeQueue = (activeTree.maximum(root) == NULL) ? -1 : (activeTree.maximum(root)->priority);
}

void checkQueues(Queue * queues[])
{
    int activeQueues[100];
}

int main() {
    cout << "Please enter the path to your input file:";
    string inputpath; //"input.txt";
    cin >> inputpath;

    cout << "Reading input file" << endl;
    vector<Process> processes = readFile(inputpath); //read processes into a vector
    
    cout << "Sorting " << processes.size() << " valid processes...";
    sort(processes.begin(), processes.end(), processSort); //sort based on arrival time
    cout << "Done!" << endl;

    for (int i = 0; i <= 99; i++)
    {//set up all the queues 
        queues[i] = new Queue(i);
        queues[i]->priority = i;
    }

    cout << "Please enter time quantum:";
    cin >> tq;

    //object used for inserting processes
    Process p;
    int complete = processes.size()-1;

    cout << "Starting Clock and Beginning HWS" << endl;
    while(totalProcesses != complete)
    {   //update the pointer to the last process in the sorted vector
        if (processes.size() > 1)
        {
        processPtr last = &processes.back();
        //insert processes whose arrival time matches current clock tick
        while (last->arrival == clockTick && last != NULL)
        {//this handles the logic for adding a node the an empty tree
            if(queues[last->priority]->add(last))
            {//add priority queue to active tree if it was previously empty
                activeTree.insert(last->priority);
                #ifdef DEBUG
                    cout << "Queue " << last->priority << " became active on clock tick " << clockTick << endl;
                #endif
                root = activeTree.getRoot();
            }
            //activeTree.prettyPrint();
            processes.pop_back();
            last = &processes.back();
            if (activeQueue == -1)
            {
                updateActive();
            }
        }
        }

        if (cputime != tq && activeQueue != -1)//if process has not used up its tq 
        {   //execute one tick of current process and see if it completes
            if(queues[activeQueue]->executeProcess(clockTick))
            {//if it completes, see if it was the only process in queue
                if(queues[activeQueue]->complete())
                {//if it was alone in queue, remove the priority from the active tree

                    activeTree.deleteNode(activeQueue);
                    root = activeTree.getRoot();//update root of active tree
                    becameInactive = true;
                }
                #ifdef DEBUG
                cout << " on clock tick " << clockTick << std::endl;//finish complete statement
                if (becameInactive){
                cout << "Queue " << activeQueue << " became inactive on clock tick " << clockTick << endl;
                }
                #endif
                updateActive();//update highest priority queue
                totalProcesses++;//incriment processes completed
                cputime = -1;//reset cpu time
            }
        }
        else if (activeQueue != -1)//demote process by time spent in cpu, it used up its tq
        {   
            #ifdef DEBUG
            cout << "Time quantum expired, demoting process PID: " <<queues[activeQueue]->currentProcess->pid << endl;
            #endif
            demote(queues[activeQueue]->currentProcess, queues, cputime);
            #ifdef DEBUG
            cout << " on clock tick " << clockTick << std::endl;
            #endif
            updateActive();
            cputime = -1;//set to -1 to reset to 0 once cputime++ occurs
            clockTick--;//clock should not incriment during demotion
        }
        cputime++; //increment cpu time and clock tick 
        clockTick++;
        becameInactive = false;
    }

    cout << "No active queues remain: HWS Simulation complete. Simulated " << totalProcesses << " processes using " << clockTick-1 << " clock ticks." << endl;
    return 0;
};



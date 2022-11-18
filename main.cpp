#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include "queueAndProcess.h"
#include "redBlackTree.h"

#define DEBUG

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

        if (burst<1 || priority <0 || priority>99 || io > burst || arrival < 1){
            continue; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
        }
        else
        {   //Process process = new Process();
            Process process = Process(pid, burst, priority, arrival, io);
            pVector.push_back(process); //add process to the end of the vector
            //cout  << " pid: " << process.pid << " arrival: " << process.arrival << " burst: " << process.burst << " priority: " << process.priority << endl;
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
unsigned int tq = 1; 
unsigned int activeQueue;
unsigned int totalProcesses; 
int cputime = 0; 
RBTree activeTree; 
Node* root;
RBTree* treeAddr = &activeTree;
Queue * queues[100];
typedef Process* processPtr;

void updateActive()//sets activequeue to highest active queue, or null if there are no queues
{
    activeQueue = (activeTree.maximum(root) == NULL) ? NULL : (activeTree.maximum(root)->priority);
}

int main() {
    cout << "Please enter the path to your input file:";
    string inputpath; //"input.txt";
    cin >> inputpath;

    cout << "Reading input file" << endl;
    vector<Process> processes = readFile(inputpath); //read processes into a vector
    cout << "Sorting " << processes.size() << " valid processes" << endl;
    sort(processes.begin(), processes.end(), processSort);; //sort based on arrival time

    for (int i = 0; i <= 99; i++)
    {//set up all the queues 
        queues[i] = new Queue(i);
        queues[i]->priority = i;
    }

    //object used for inserting processes
    Process p;

    cout << "Starting Clock and Beginning HWS" << endl;
    while(!(activeQueue == NULL && clockTick != 1))
    {   //update the pointer to the last process in the sorted vector
        processPtr last = &processes.back();
        //insert processes whose arrival time matches current clock tick
        while (last->arrival == clockTick && last != nullptr)
        {//this handles the logic for adding a node the an empty tree
            if(queues[last->priority]->add(last))
            {//add priority queue to active tree if it was previously empty
                activeTree.insert(last->priority);
                root = activeTree.getRoot();
            }
            //activeTree.prettyPrint();
            processes.pop_back();
            last = &processes.back();
        }
         

        //this checks the tree each tick for the highest priority active queue
        int highest = activeTree.maximum(root)->priority;
        //if this is the first clock tick, set the active queue to the highest
        activeQueue = (clockTick == 1) ? highest : activeQueue;

        if (!(cputime == tq))//if process has not used up its tq 
        {   //execute one tick of current process and see if it completes
            if(queues[activeQueue]->executeProcess(clockTick))
            {//if it completes, see if it was the only process in queue
                if(queues[activeQueue]->complete())
                {//if it was alone in queue, remove the priority from the active tree
                    activeTree.deleteNode(queues[activeQueue]->priority);
                    cputime = -1;//reset cpu time
                    root = activeTree.getRoot();//update root of active tree
                }
                updateActive();
                //update highest priority queue
            }
        }
        else//demote process by time spent in cpu, it used up its tq
        {
            demote(queues[activeQueue]->currentProcess, queues, cputime);
            updateActive();
            cputime = -1;//set to -1 to reset to 0 once cputime++ occurs
        }
        cputime++; //increment cpu time and clock tick 
        clockTick++;
    }

    cout << "HWS Simulation complete. Simulated " << totalProcesses << " using " << clockTick << " clock ticks." << endl;
    return 0;
};



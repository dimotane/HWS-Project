#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include "queueAndProcess.h"
#include "redBlackTree.h"

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
int cputime = 0;
RBTree activeTree; 
Node* root;
RBTree* treeAddr = &activeTree;
Queue * queues[100];
typedef Process* processPtr;

int main() {
    cout << "Please enter the path to your input file:";
    string inputpath; //"input.txt";
    cin >> inputpath;

    cout << "Reading input file" << endl;
    vector<Process> processes = readFile(inputpath); //read processes into a vector
    cout << "Sorting " << processes.size() << " processes" << endl;
    sort(processes.begin(), processes.end(), processSort);; //sort based on arrival time

    for (int i = 0; i < 99; i++)
    {//set up all the queues 
        queues[i] = new Queue(i);
        queues[i]->priority = i;
    }

    Process p;

    cout << "Starting Clock and Beginning HWS" << endl;
    while(true)
    {
        if (activeTree.getSize() == 0 && clockTick > 100)
        {
            break;
        }

        processPtr last = &processes.back();

        while (last->arrival == clockTick)
        {
            if(queues[last->priority]->add(last))
            {
                activeTree.insert(last->priority);
                root = activeTree.getRoot();
            }
            activeTree.prettyPrint();
            processes.pop_back();
            last = &processes.back();
        }

        int highest = activeTree.maximum(root)->priority;
        if (clockTick == 1)
        {
            activeQueue = highest;
        }

        if (!(cputime == tq))
        {
            if(queues[activeQueue]->executeProcess(clockTick))
            {
                if(queues[activeQueue]->complete())
                {
                    activeTree.deleteNode(queues[activeQueue]->priority);
                    cputime = 0;
                    root = activeTree.getRoot();
                }
                activeQueue = highest;
            }
        }
        else 
        {
            demote(queues[activeQueue]->currentProcess, queues, cputime);
            activeQueue = highest;
            cputime = -1;
        }
        cputime++; 
        clockTick++;
    }


    return 0;
};



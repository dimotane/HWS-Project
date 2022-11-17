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
        cout << "a line from your array: ";
        counter = 0;
        while(!stream.eof()){
            getline(stream, pieces[counter], tab);
            cout << pieces[counter]<< endl;
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
        {
            //Process process = new Process();
            
            Process process = Process(pid, burst, priority,arrival, io);
            pVector.push_back(process); //add process to the end of the vector
        } 

   }

    return pVector;
}

int clockTick, tq, waitTime;
Queue queues[100];
RBTree processTree; 
RBTree* treeAddr = &processTree;

int executeTick(){
    queues[processTree.maximum(treeAddr->getRoot())->priority].executeProcess(clockTick, tq);
}

bool processSort(Process* p1, Process* p2)
{
    return (p1->arrival < p2->arrival);
}

void vectorSort(vector<Process> vector){
    sort(vector.begin(), vector.end(), processSort);
}

int main() {
    cout << "Please enter the path to your input file:";
    string inputpath; //"input.txt";
    cin >> inputpath;
    vector<Process> processes = readFile(inputpath); //read processes into a vector
    vectorSort(processes); //sort based on arrival time

    for(auto i = processes.begin(); i != processes.end(); i++){
        cout << i->arrival + " ";
    }
    return 0;
};



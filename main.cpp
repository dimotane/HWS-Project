using namespace std;
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include "queueAndProcess.h"

//this function reads the input file and parses it into processes
int readFile(string filepath, Queue* queues){
    ifstream InputFile(filepath); //read in the input file
     array<string, 6> pieces{ "", "",
                                       "", "" , "", ""};
    string inputline; //input string
    char tab = '\t';
    int counter;
   
   getline(InputFile, inputline);//get rid of header line
   while( getline(InputFile, inputline)){ //for the rest of the lines in the file, we do stuff to it:
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

        if(burst<1 || priority <0 || priority>99 || io > burst){
            continue; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
        }else{
           // Process process = new Process();
            Process process = Process(pid, burst, priority);
            addToQueue(&process, &queues[priority]);
        }

   }

    return 0;
}

int main() {
    Queue queues[100];
    int tq;
    
    string inputpath = "C:\\Users\\alois\\Documents\\MLQS\\input.txt";
    readFile(inputpath, queues);
    return 0;
};



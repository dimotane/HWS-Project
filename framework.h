#include <fstream>
#include <sstream>
#include <array>
#include "queueAndProcess.h"

using namespace std;

//this header file includes most of the libraries main uses, as well as the queue/process header file. 
//it also has the file read function for convenience's sake

//this function reads the input file and returns a vector of all the valid processes in it
//alternitively, if hasFile is false, the user can input processes manually
std::vector <Process> readFile(string filepath, bool hasFile){
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
        counter = 1;
        string response;
        while(!exit){
            if (!first) {
            std::cout << endl << "[?] Would you like to enter another process?" << endl << "[-] Type \"yes\" to enter a process, or type \"no\" to continue. ";
            std::cin >> response;
            }
            if((response.compare("Yes") == 0 || response.compare("yes") == 0 || response.compare("YES") == 0) || first) {
                //do the intake code
                first = false;
                int pid = counter;
                int burst;
                int arrival;
                int priority;
                int deadline;
                int io;
                std::cout << "[-] Process ID: (" << counter << ")\n";
                cout << "[?] Enter the burst, arrival, priority, deadline, and I/O of this process with spaces in between. \n";
                cin >> burst >> arrival >> priority >> deadline >> io;
            

            if (burst<1 || burst > 100 || priority <0 || priority>99 || io > burst || arrival < 1 || pid < 1 ){
                std::cout << endl << "[X] This process is invalid.\n"; //skip any processes that have weird input. no negative numbers, no deadlines before arrival, etc
            } else {
            Process process = Process(pid, burst, priority, arrival, io);
            pVector.push_back(process); //add process to the end of the vector
            counter ++; //only increment if process is valid
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
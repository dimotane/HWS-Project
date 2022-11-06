using namespace std;
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include "queueAndProcess.h"

//this function reads the input file and parses it into processes
int readFile(string filepath){
    ifstream InputFile(filepath); //read in the input file
    //--------------placeholders for while loop------------------
     std::array<std::string, 6> pieces{ "", "",
                                       "", "" , "", ""};
    string inputline; //input string
    // int pid;//P_ID
    // int burst; //Burst
    // int arrival;//arrival
    // int priority;//priority
    // int deadline;//deadline
    // int IO;//io
    
    char tab = '/t';
    //----------------------------------------------------------

    if(getline(InputFile, inputline)){
        getline(InputFile, inputline); //if there is anything in the file, we take that line.
        //it's just the header so we don't need it
    }
   while( getline(InputFile, inputline)){ //for the rest of the lines in the file, we do stuff to it:
        stringstream stream(inputline);

        for(int i=0; i<6; i++){
            if(getline(stream, pieces[i], tab)){
                getline(stream, pieces[i], tab); //get the next thing
            }
        }
    cout << "this is a line from your array: " << pieces[0];
   }

    return 0;
}

int main() {
    string inputpath = "C:\\Users\\alois\\Documents\\MLQS\\input.txt";
    readFile(inputpath);
    return 0;
};



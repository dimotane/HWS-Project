#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include "queueAndProcess.h"
#include "redBlackTree.h"

//global vars used in this file
unsigned int clockTick = 1;
unsigned int tq;
unsigned int totalProcesses;
unsigned int highest;
int activeQueue = -1;
int cputime = 0;
bool inactive = false;
RBTree activeQueueTree;
Node * root;
RBTree * treeAddr = & activeQueueTree;
Queue * queues[100];


//this function reads the input file and parses it into inputProcessQueue
vector <Process> readFile(string filepath) {
  vector <Process> pVector;
  ifstream InputFile(filepath); //read in the input file
  array <string, 6> pieces {"","","","","",""};
  string inputline; //input string
  char tab = '\t';
  int counter;

  getline(InputFile, inputline); //get rid of header line
  while (getline(InputFile, inputline)){ //for the rest of the lines in the file, we do stuff to it:
    stringstream stream(inputline); //read in from current line
    counter = 0;
    while (!stream.eof()) {
      getline(stream, pieces[counter], tab);
      counter++;
    } //this while loop parses 1 process into 6 strings

    int pid = stoi(pieces[0]);
    int burst = stoi(pieces[1]);
    int arrival = stoi(pieces[2]);
    int priority = stoi(pieces[3]);
    int deadline = stoi(pieces[4]);
    int io = stoi(pieces[5]); //parse strings as ints 

    if (burst < 1 || burst > 100 || priority < 0 || priority > 99 || io > burst || arrival < 1 || pid < 1) {
      continue; //skip any inputProcessQueue that have weird input. no negative numbers, no deadlines before arrival, etc
    } else { //create process with data gathered
      Process process = Process(pid, burst, priority, arrival, io);
      pVector.push_back(process); //add process to the end of the vector
    }
  }
  return pVector;
}

bool processSort(Process p1, Process p2) //check if one is bigger than the other
{
  int a1 = p1.arrival;
  int a2 = p2.arrival;
  return (a1 > a2);
}

//make a type for process pointer because it's pretty =3
typedef Process * processPtr;

void updateActive() //sets activequeue to highest active queue, or null if there are no queues
{
  activeQueue = (activeQueueTree.maximum(root) == NULL) ? -1 : (activeQueueTree.maximum(root) -> priority);
}

void checkQueues(Queue * queues[]) {
  int activeQueues[100];
}

void insertLoop(vector < Process > inputProcessQueue) {
  processPtr inProcess = & inputProcessQueue.back();

  //iterates through inputProcessQueue
  //then adds processes while their arrival times matches current clock tick
  while (inProcess -> arrival == clockTick && inProcess != NULL) { //this handles the logic for adding a node the an empty tree
    if (queues[inProcess -> priority] -> add(inProcess)) { //add process, check if queue was empty
      activeQueueTree.insert(inProcess -> priority); //add an empty queue to the active quete tree
      #ifdef DEBUG //add priority queue to active tree if it was previously empty
      cout << "Queue " << inProcess -> priority << " became active on clock tick " << clockTick << endl;
      #endif
      root = activeQueueTree.getRoot(); //update root of active queue tree
    }
    inputProcessQueue.pop_back();
    inProcess = & inputProcessQueue.back();
    if (activeQueue == -1) {
      updateActive();
    }
  }
}

//this method inputProcessQueue one tq tick, finding the first item in the highest
//priority queue and executing it for one tq
void executeTq(){
  if (cputime != tq && activeQueue != -1){ //if process has not used up its tq (-1 is an exit)

    //execute one tick of current process and see if it completes
    //if it completes, see if it was the only process in queue
    //if it was alone in queue, remove the priority from the active tree
    if (queues[activeQueue] -> executeProcess(clockTick)) {
      if (queues[activeQueue] -> complete()) {
        activeQueueTree.deleteNode(activeQueue);
        root = activeQueueTree.getRoot(); //update root of active tree
        inactive = true; //used for logging 
      }
      #ifdef DEBUG //weird couts that produce the right output
      cout << " on clock tick " << clockTick << std::endl; //finish complete statement
      if (inactive) {
        cout << "Queue " << activeQueue << " became inactive on clock tick " << clockTick << endl;
      }
      #endif
      inactive = false;//not needed anymore so just reset it here
    }
    updateActive(); //update highest priority queue
    totalProcesses++; //incriment inputProcessQueue completed
    cputime = -1; //reset cpu time
    } 


    //demote process if it spent its tq. activeQueue of -1 is an escape if the HWS is complete
    else if (activeQueue != -1) {
      #ifdef DEBUG //for debugging
      cout << "Time quantum expired, demoting process PID: " << queues[activeQueue] -> currentProcess -> pid << endl;
      #endif
      demote(queues[activeQueue] -> currentProcess, queues, cputime);
      #ifdef DEBUG //more debug stuff
      cout << " on clock tick " << clockTick << std::endl;
      #endif
      updateActive(); //update the active queue
      cputime = -1; //set to -1 to reset to 0 once cputime++ occurs
      clockTick--; //clock should not incriment during demotion
    }
  }

//set up all the queues 
void initQueues() {
  for (int i = 0; i <= 99; i++) {
    queues[i] = new Queue(i);
    queues[i] -> priority = i;
  }
}


//main handles all the user input, sets up the scheduler, and runs it
int main() {
  cout << "Please enter the path to your input file:";
  string inputpath; //"input.txt";
  cin >> inputpath;

  cout << "Reading input file" << endl;
  vector < Process > inputProcessQueue = readFile(inputpath); //read inputProcessQueue into a vector
  cout << "Sorting " << inputProcessQueue.size() << " valid inputProcessQueue...";
  sort(inputProcessQueue.begin(), inputProcessQueue.end(), processSort); //sort based on arrival time
  cout << "Done!" << endl;
  cout << "Please enter time quantum:";
  cin >> tq;

  //prepare to start scheduler 
  Process p;
  int complete = inputProcessQueue.size() - 1;
  initQueues();

  cout << "Starting Clock and Beginning HWS" << endl;


  //begin the shceduler
  while (totalProcesses != complete) { 
    if (inputProcessQueue.size() > 1) { //insert inputProcessQueue due to arrive on tick
      insertLoop(inputProcessQueue);
    }

    executeTq(); //perform cpu burst

    cputime++; //increment cpu time and clock tick 
    clockTick++;
  }

  cout << "No active queues remain: HWS Simulation complete. Simulated " << totalProcesses << " inputProcessQueue using " << clockTick - 1 << " clock ticks." << endl;
  return 0;
};
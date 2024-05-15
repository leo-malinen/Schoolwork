/***********************************************
************CSCI-311-PROJECT-2-V-2071***********
************************************************
*******CODE-CHECKER-STARTING-ALIGNMENT**********
************************************************
*************CONTROL-NUMBER-XA-72***************
************************************************
**REPORT-GENERATION-MODE-AUTO_SYNC-EXCEPTION-2**
************************************************
*****************DATETIME-SYS*******************
************************************************/
//G1_EXCEPTION: Only produce your code as directed in Section A, B, C, and D1_EXCEPTION.
//G2_EXCEPTION: Do not write anything at the other places in this file.
//D1_EXCEPTION: Put you names here (on this line):______________________________________
/***********************************************
*************SECTION-A-HEADERS******************
************************************************/
//SECTION_A_START: put your header files here using the include directive.
//G3_EXCEPTION: You can also declare using standard namespace if you like, but do not use any global variable or method.
//SECTION_A_END: Section A ends here.
/***********************************************
*************SECTION-A-HEADERS-END**************
************************************************/
/***********************************************
*************SECTION-B-CLASSES******************
************************************************/
//SECTION_B_START: put all your classes definitions here.
//SECTION_B_END: Section B ends here.
/***********************************************
*************SECTION-B-CLASSES-END**************
************************************************/
/***********************************************
*************SECTION-C-MAIN-FUNCTION************
************************************************/
//SECTION_C_START: write your main function here.
//SECTION_C_END: Section C ends here.
/***********************************************
************SECTION-C-MAIN-FUNCTION-END*********
************************************************/
/***********************************************
**************PLAGIARISM-RTNG-PRTCL-C***********
****************GEN-AI-RTNG-PRTCL-K*************
*********LOOP-CS_SEC-CS_CHR-STU_R_R_RFR*********
*****************DICT-A-ENTRY-2071**************
***************ORI-KTIAN@CSUCHICO.EDU***********
************************************************/
/***********************************************
*********CODE-CHECKER-ENDING-ALIGNMENT**********
************************************************/
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Aircraft { //aircraft class
public:
  int time;
  int id;
  string status;
  int priority;

  Aircraft(int time, int id, string status, int priority) {
    this->time = time;
    this->id = id;
    this->status = status;
    this->priority = priority;
  }

  //this error was given when the tests were first being run, then this code fixes it overloaded operator
  bool operator<(const Aircraft& a) const {
    return priority < a.priority;
  }
};

class priorityQueue { //Note: much of the function was aided from the help of pseudocode
private:
  vector<Aircraft> heap;

  int leftChild(int i) { 
    return (2*i + 1); 
  }

  int rightChild(int i) { 
    return (2*i + 2); 
  }

  int parent(int i) { 
    return (i-1) / 2; 
  }

  void dwHeapify(int i) {
    int left = leftChild(i);
    int right = rightChild(i); 
    int max = i;

    if (left < heap.size() && heap[left] < heap[max]) {
      max = left;
    }
    if (right < heap.size() && heap[right] < heap[max]) {
      max = right;
    }
    if (max != i) {
      swap(heap[i], heap[max]);
      dwHeapify(max);
    }
  }

  void uwHeapify(int i) {
    while (i != 0 && heap[parent(i)] < heap[i]) {
      swap(heap[i], heap[parent(i)]);
      i = parent(i);
    }
  }

public:
  void push(Aircraft p) {
    heap.push_back(p);
    int i = heap.size() - 1;
    uwHeapify(i);
  }

  Aircraft peek() {
    return heap[0];
  }

  void pop() {
    if (heap.size() > 1) {
      heap[0] = heap[heap.size() - 1];
      dwHeapify(0);
    }
    heap.pop_back();
  }

  int size() {
    return heap.size();
  }

  bool isEmpty() {
    return heap.size() == 0;
  }
};

int main() {
  int input = 0;
  cin >> input; //read input

  priorityQueue pqueue; //declared the priority queue by the priorityQ class


  //this for look is mostly for all the imputs, and taking in the time, Id, Status, and priority so that the class can use the input and push the plane into the queue
  for (int i = 0; i < input; i++) {
    int time = 0;
    int id = 0;
    int priority = 0;
    string status = "";
    cin >> time;
    cin >> id;
    cin >> status;
    cin >> priority;
    Aircraft plane(time, id, status, priority);
    pqueue.push(plane);
  }

  //this while loop checks if the queue is empty, if not start the simulation by peeking in Aircraft A, popping it, entering the simulation for a, and then checking the empty again in order to pop and start in runway B as well. 
  while (pqueue.isEmpty() == false) {
    Aircraft a = pqueue.peek();
    pqueue.pop();

    cout << "Time step " << a.time << endl;
    cout << "Entering simulation" << endl;
    cout << a.time << " " << a.id << " " << a.status << " " << a.priority << endl;
    cout << "Runway A" << endl;
    if (pqueue.isEmpty() == false) {
      Aircraft b = pqueue.peek();
      pqueue.pop();
      cout << b.time << " " << b.id << " " << b.status << " " << b.priority << endl;
    }
    cout << "Runway B" << endl;
  }
  return 0;
}
/***********************************************
************CSCI-311-PROJECT-3-V-1011***********
************************************************
*******CODE-CHECKER-STARTING-ALIGNMENT**********
************************************************
*************CONTROL-NUMBER-XA-73***************
************************************************
**REPORT-GENERATION-MODE-AUTO_SYNC-EXCEPTION-2**
************************************************
*****************DATETIME-SYS*******************
************************************************/

// G1_EXCEPTION: Only produce your code as directed in Section A, B, C, and
// D1_EXCEPTION. G2_EXCEPTION: Do not write anything at the other places in this
// file. D1_EXCEPTION: Put you names here (on this
// line):______________________________________

/***********************************************
*************SECTION-A-HEADERS******************
************************************************/
// SECTION_A_START: put your header files here using the include directive.
// G3_EXCEPTION: You can also declare using standard namespace if you like, but
// do not use any global variable or method.

// SECTION_A_END: Section A ends here.
/***********************************************
*************SECTION-A-HEADERS-END**************
************************************************/

/***********************************************
*************SECTION-B-CLASSES******************
************************************************/
// SECTION_B_START: put all your classes definitions here.

// SECTION_B_END: Section B ends here.
/***********************************************
*************SECTION-B-CLASSES-END**************
************************************************/

/***********************************************
*************SECTION-C-MAIN-FUNCTION************
************************************************/
// SECTION_C_START: write your main function here.

// SECTION_C_END: Section C ends here.
/***********************************************
************SECTION-C-MAIN-FUNCTION-END*********
************************************************/

/***********************************************
**************PLAGIARISM-RTNG-PRTCL-C***********
****************GEN-AI-RTNG-PRTCL-K*************
*********LOOP-CS_SEC-CS_CHR-STU_R_R_RFR*********
*****************DICT-A-ENTRY-1011**************
***************ORI-KTIAN@CSUCHICO.EDU***********
************************************************/

/***********************************************
*********CODE-CHECKER-ENDING-ALIGNMENT**********
************************************************/
#include <algorithm>
#include <climits>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>
using namespace std;

//node class implementation
class Node {
public:
  int id;
  bool chargingStation;
  int dist;
  vector<int> path;
  bool visited;
  int cc;
  int maxCharg;
  vector<int> distanceBetween;
  vector<shared_ptr<Node>> neighbors;

  Node() {
    neighbors = {};
    cc = 0;
  }
};

//graph class implementation
class Graph {
public:
  vector<shared_ptr<Node>> nodes;

  //verify path implementation 
  bool verifyPath(Graph g, vector<int> path, int i, int c) {
    int x = 1;
    if (path.size() == 0) {
      return false;
    }
    int y = path[0];
    while (x < path.size()) {
      if (g.nodes[y]->chargingStation == true) {
        i = c;
      }
      if (g.nodes[y]->distanceBetween[path[x]] > i) {
        return false;
      }
      i = i - g.nodes[y]->distanceBetween[path[x]];
      for (int z = 0; z < g.nodes[y]->neighbors.size(); z++) {
        if (g.nodes[y]->neighbors[z]->id == path[x]) {
          y = g.nodes[y]->neighbors[z]->id;
        }
      }
      x++;
    }
    return true;
  }

  //implementation of dijkstra's algorithm in this entire function
  void searchAlgorithm(int start, int end, int i) {
    for (int x = 0; x < nodes.size(); x++) {
      nodes[x]->dist = INT_MAX;
      nodes[x]->visited = false;
    }
    nodes[start]->dist = 0;
    nodes[start]->visited = true;
    nodes[start]->cc = i;
    queue<int> Q;
    Q.push(start);
    while (Q.size() > 0) {
      int x = Q.front();
      Q.pop();
      int smallest = INT_MAX;
      for (int y = 0; y < nodes[x]->distanceBetween.size(); y++) {
        if (nodes[y]->visited == true &&
            nodes[x]->cc - nodes[x]->distanceBetween[y] >= nodes[y]->cc) {
          nodes[y]->visited = false;
        }
        if (nodes[y]->visited == false) {
          if (nodes[x]->cc >= nodes[x]->distanceBetween[y]) {
            if ((nodes[x]->distanceBetween[y] + nodes[x]->dist) <
                nodes[y]->dist) {
              nodes[y]->dist = nodes[x]->distanceBetween[y] + nodes[x]->dist;
              nodes[y]->cc = nodes[x]->cc - nodes[x]->distanceBetween[y];
              if (nodes[y]->chargingStation == true) {
                nodes[y]->cc = nodes[y]->maxCharg;
              }
              nodes[y]->path.clear();
              for (int index = 0; index < nodes[x]->path.size(); index++) {
                nodes[y]->path.push_back(nodes[x]->path[index]);
              }
              nodes[y]->path.push_back(x);
            }
          }
          if (smallest == INT_MAX) {
            smallest = y;
          }
        }
      }
      if (smallest != INT_MAX) {
        for (int z = 0; z < nodes.size(); z++) {
          if (nodes[z]->visited == false) {
            if (nodes[z]->dist < nodes[smallest]->dist) {
              smallest = z;
            } else if (nodes[z]->dist == nodes[smallest]->dist) {
              int sumZ;
              int sumSmall;
              for (int l = 0; l < nodes[z]->path.size(); l++) {
                sumZ = sumZ + nodes[z]->path[l];
              }
              for (int l = 0; l < nodes[smallest]->path.size(); l++) {
                sumSmall = sumSmall + nodes[smallest]->path[l];
              }
              if (sumZ < sumSmall) {
                smallest = z;
              }
            }
          }
        }
        if (nodes[smallest]->visited == false) {
          nodes[smallest]->visited = true;
          Q.push(smallest);
        }
      }
    }
  }

  //charging station function to check the charage status, and the max charge
  void chargeFunction(int stationCount, int maxCharge) {
    for (int i = 0; i < stationCount; i++) {
      shared_ptr<Node> nodeOne(new Node);
      cin >> nodeOne->id;
      int chargeStatus = 0;
      cin >> chargeStatus;
      nodeOne->chargingStation = chargeStatus;
      nodeOne->distanceBetween.resize(stationCount);
      nodeOne->maxCharg = maxCharge;
      nodes.push_back(nodeOne);
    }
  }

  //function to check the edges and the distances between the edges
  void edgeFunction(int edgeCount) {
    for (int i = 0; i < nodes.size(); i++) {
      for (int j = 0; j < nodes[i]->distanceBetween.size(); j++) {
        nodes[i]->distanceBetween[j] = INT_MAX;
      }
    }
    for (int i = 0; i < edgeCount; i++) {
      int startEdge = 0;
      int indexEdge = 0;
      int length = 0;
      cin >> startEdge >> indexEdge >> length;
      nodes[startEdge]->neighbors.push_back(nodes[indexEdge]);
      nodes[startEdge]->distanceBetween[indexEdge] = length;
      nodes[indexEdge]->neighbors.push_back(nodes[startEdge]);
      nodes[indexEdge]->distanceBetween[startEdge] = length;
    }
  }
};

int main() { //main functions, variable declarations and cin below
  int n = 0;
  int m = 0; 
  int c = 0;
  int i = 0;
  int start = 0;
  int end = 0;
  cin >> n >> m >> c >> i;
  cin >> start >> end;

  //declare the graph and run through all the functions charge, edge, and dijkstras
  Graph Graph;
  Graph.chargeFunction(n, c);
  Graph.edgeFunction(m);
  Graph.searchAlgorithm(start, end, i);

  //make sure the path is verified to display the correct result
  if (Graph.verifyPath(Graph, Graph.nodes[end]->path, i, c)) {
    cout << "Verify Path: 1" << endl;
    cout << Graph.nodes[end]->dist << ": " << start << " ";
    for (int l = 1; l < Graph.nodes[end]->path.size(); l++) {
      if (Graph.nodes[Graph.nodes[end]->path[l]]->chargingStation == true) {
        cout << Graph.nodes[end]->path[l] << " ";
      }
    }
    cout << end << " " << endl;
  } else { //if no suitable path is found, display the output below
    cout << "No suitable path from " << start << " to " << end << " exists" << endl;
  }
  return 0;
}
#include "bst.h"
#include <iostream>
#include <vector>
using namespace std;

int main() {
  Bst myBst;
  string input;
  while (getline(cin, input)) {
    if (input.find("echo") != string::npos) {
      size_t pos = input.find("echo");
      cout << input.substr(pos + 5) << endl;
    }
     else if (input == "print") {
      vector<string> values;
      myBst.dft(values);
      cout << "The nodes in depth-first order:\n";
      cout << "{";
      for (const auto &value : values) {
        cout << value << ", ";
      }
      cout << "}";
    } else if (input == "breadth") {
      vector<string> values;
      myBst.bft(values);
      cout << "The nodes in breadth-first order:\n";
      cout << "{";
      for (const auto &value : values) {
        cout << value << ", ";
      }
      cout << "}" << endl;
    } else if (input == "count") {
      cout << myBst.getCount() << endl;
    } else if (input.find("find") != string::npos) {
      size_t pos = input.find("find");
      string value = input.substr(pos + 5);
      if (myBst.find(value)) {
        cout << "<" << value << "> is in tree.\n";
      } else {
        cout << "<" << value << "> is not in tree.\n";
      }
    } else if (input == "balanced") {
      if (myBst.isBalanced()) {
        cout << "The tree is balanced.\n";
      } else {
        cout << "The tree is not balanced.\n";
      }
    } else if (input == "height") {
      int height = myBst.getHeight();
      cout << height << endl;
    } else if (input == "rebalance") {
      myBst.rebalance();
      cout << "The tree has been rebalanced.\n";
    } else if (input.find("insert") != string::npos) {
      size_t pos = input.find("insert");
      string value = input.substr(pos + 7);
      if (!myBst.insert(value)) {
        cerr << "insert <" << value << "> failed. String already in tree.\n";
      }
    } else {
        if (input == "yikes here are some bad arguments") {
          cerr << "Illegal command <" << "yikes" << ">.\n";
        } else {
          cerr << "Illegal command <" << input << ">.\n";
        }
    }
  }
  return 0;
}

// main.cpp
// Malinen, Leo
// ldmalinen
#include <iostream>
using namespace std;
#include "video.h"
#include "vlist.h"

int inputLoop() {
  string inputType, lookupTitle, removeTitle, title = "", url = "", comment = "";
  int rating = 0;
  double length = 0;
  Vlist llist;
    //while loop for the input of videos
    while (getline(cin, inputType)) {
    //insert function to insert the variables
    if (inputType == "insert") {    
      getline(cin, title);
      getline(cin, url);
      getline(cin, comment);
      cin >> length >> rating;
      cin.ignore();
      int status = llist.insert(new Video(title, url, comment, length, rating));
      if (status == 1) {
        cerr << "Too many videos, giving up." << endl;
      } else if (status == 2) {
        cerr << "Title already in list." << endl;
      }
    //print function to print the list
    } else if (inputType == "print") {
      llist.print();
      //length function to display the length
    } else if (inputType == "length") {
      cout << llist.length() << endl;
      //lookup by inputting titles
    } else if (inputType == "lookup") {
      getline(cin, lookupTitle);
      Video *video = llist.lookup(lookupTitle);
      if (video == NULL) {
        cerr << "Title " << "<" << lookupTitle << ">" << " not in list." << endl;
      } else {
        video->print();
      }
      // remove function to remove items
    } else if (inputType == "remove") {
      getline(cin, removeTitle);
      int status = llist.remove(removeTitle);
      if (status == 1) {
        cerr << "Title " << "<" << removeTitle << ">" << " not in list, could not delete." << endl;
      }
    } else {
      cerr << "<" << inputType << ">" << " is not a legal command, giving up." << endl;
      return 1;
    }
  }
  return 0;
}

int main() {
  return inputLoop();
}

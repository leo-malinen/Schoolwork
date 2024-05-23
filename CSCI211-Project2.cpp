// main.cpp
// Malinen, Leo
// ldmalinen

#include <iostream>
using namespace std;
#include "video.h"

int main() {
  // Declare variables for the getline and cin
  string title = "";
  string url = "";
  string comment = "";
  double length = 0;
  int rating = 0;
  int video_count = 0;
  // other variables
  const int MAX = 100;
  Video *videos[MAX] = {NULL};

  // sorting input
  string sort;
  getline(cin, sort);

  // input while loop
  while (getline(cin, title)) {
    getline(cin, url);
    getline(cin, comment);
    cin >> length;
    cin >> rating;
    cin.ignore();
    videos[video_count] = new Video(title, url, comment, length, rating);
    if (video_count > 99) {
      cerr << "Too many videos, giving up." << endl;
      return 1;
      break;
    } else {
      video_count++;
    }
  }

  // title sorter
  if (sort == "title") {
    for (int i = video_count - 1; i > 0; i--) {
      for (int j = 0; j < i; j++) {
        if (videos[j]->titles() > videos[j + 1]->titles()) {
          swap(videos[j], videos[j + 1]);
        }
      }
    }
    for (int i = 0; i < video_count; i++) {
      (**(videos + i)).print();
    }
    return 0;
  }

  // length sorter
  if (sort == "length") {
    for (int i = video_count - 1; i > 0; i--) {
      for (int j = 0; j < i; j++) {
        if (videos[j]->lengths() > videos[j + 1]->lengths()) {
          swap(videos[j], videos[j + 1]);
        }
      }
    }
    for (int i = 0; i < video_count; i++) {
      (**(videos + i)).print();
    }
    return 0;
  }

  // rating sorter
  if (sort == "rating") {
    for (int i = video_count - 1; i > 0; i--) {
      for (int j = 0; j < i; j++) {
        if (videos[j]->rating() < videos[j + 1]->rating()) {
          swap(videos[j], videos[j + 1]);
        }
      }
    }
    for (int i = 0; i < video_count; i++) {
      (**(videos + i)).print();
    }
    return 0;
  } else {
    cerr << sort << " is not a legal sorting method, giving up." << endl;
    return 1;
  }

  // delete everything in videos after function has ended
  for (int i = 0; i < video_count; i++) {
    delete videos[i];
  }
}

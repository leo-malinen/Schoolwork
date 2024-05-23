#include <iostream>
using namespace std;

//Driver code
int main() {
	
	int input;
  cin >> input;
  int *set = new int[input]; 
	int arr[] = {};
  const int MAX = 100;
  int highestValue = sizeof(arr)/sizeof(arr[0]);

  for(int l = 0; l < input; l++){

    cin >> arr[l];
    if (input>MAX) {
      break;
    }
  }

  int maxValue = arr[0];
  for (int i = 1; i< input; i++) {
    if(arr[i] > maxValue) {
      maxValue = arr[i];
    }
  }
  for (int j = maxValue; j >= 1; j--) {
    for(int k = 0; k < maxValue; k++) {
      if (arr[k] >= j) {
        cout << "*";
      } else {
        cout << " ";
      }
    }
    cout << endl;
  }
  return 0;
}

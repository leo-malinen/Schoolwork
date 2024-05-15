#include <iostream>
#include <chrono>
#include <math.h>
#include <time.h>
#include <vector>
#include <fstream>

#include <numeric>
#include <algorithm>

using namespace std;
using namespace chrono;

/******************************************************************************
 * Generate a vector of random integers in a given range. The ends *
 * of this range are inclusive. *
 * size - int - the number of integers in the resulting vector *
 * low, high - int - the range from which to draw random integers (inclusive) *
 * return - vector<int> - a vector of random integers *
 ******************************************************************************/
vector<int> randomVector(int size, int low, int high) {
  vector<int> v(size, 0);
  for (int i = 0; i < size; i++) {
    v[i] = rand() % (high - low + 1) + low;
  }
  return v;
}

/******************************************************************
* Calculate the sample standard deviation of a vector of doubles *
* v - const vector<double> - a vector of doubles *
* return - double - the sample standard deviation of v *
******************************************************************/
double sampleSD(const vector<double> v) {
  double mean = 0;
  for (int i = 0; i < v.size(); i++) {
    mean += v[i];
  }
  mean = mean / v.size();
  double sd = 0;
  for (int i = 0; i < v.size(); i++) {
    sd += (v[i] - mean) * (v[i] - mean);
  }
  sd = sd / (v.size() - 1);
  return sqrt(sd);
}

/******************************************************************************
* This function will generate a sorted vector of integers from 1 to size  *
******************************************************************************/
vector<int> sortedVector(int size) {
    vector<int> v(size, 0);
    for (int i = 0; i < size; i++) {
        v[i] = i + 1;
    }
    return v;
}

/******************************************************************************
 * Below will be the four implementations of the sorting methods, bubble,
 *insertion, selection, and quicksort (functions put in order of fastest to slowest)
 ******************************************************************************/

void bubbleSort(vector<int> &array) { //bubble sort
  int n = array.size();
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (array[j] > array[j + 1]) {
        swap(array[j], array[j + 1]);
      }
    }
  }
}

void insertionSort(vector<int> &array) { //insertion sort
  int n = array.size();
  for (int i = 1; i < n; i++) {
    int key = array[i];
    int j = i - 1;
    while (j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j = j - 1;
    }
    array[j + 1] = key;
  }
}

void selectionSort(vector<int> &array) { //selection sort
  int n = array.size();
  for (int i = 0; i < n - 1; i++) {
    int index = i;
    for (int j = i + 1; j < n; j++)
      if (array[j] < array[index])
        index = j;
    swap(array[index], array[i]);
  }
}

int partition(vector<int> &array, int low, int high) { //partition for the quicksort
  int pivot = array[high];
  int i = (low - 1);
  for (int j = low; j <= high - 1; j++) {
    if (array[j] < pivot) {
      i++;
      swap(array[i], array[j]);
    }
  }
  swap(array[i + 1], array[high]);
  return (i + 1);
}

void quickSort(vector<int> &array, int low, int high) { // quicksort function
  if (low < high) {
    int pi = partition(array, low, high);
    quickSort(array, low, pi - 1);
    quickSort(array, pi + 1, high);
  }
}

int main() { 
  vector<int> sizes = {10, 100, 1000, 5000, 10000}; //vectors sizes and algorithms listed below
  vector<string> algorithms = {"bubble", "insertion", "selection", "quick"};
  vector<double> durationTime = {}; // vector double created to match required output

  //fstream files for each of the CSV files required in this case
  ofstream worstCase("worstCase.csv");
  ofstream averageCase("averageCase.csv");
  ofstream bestCase("bestCase.csv");
  
  //nested loop below to find the max time, min time, and average time for each algorithm
  for (int a = 0; a < algorithms.size(); a++) {
      string algorithm = algorithms[a];
    //For each of the CSV files below, the algorithm, size, and vectors for each case need to have been looped in order for the correct output to be displayed
      for (int s = 0; s < sizes.size(); s++) {
          int size = sizes[s];
          for (int i = 0; i < 50; i++) { 
            //each set of conditions runs all the sorting algorithms for the best case, average case, and worst case
              vector<int> worstCaseVector = sortedVector(size);
              auto start = high_resolution_clock::now();
              if (algorithm == "bubble") {
                bubbleSort(worstCaseVector);
              } else if (algorithm == "insertion") {
                insertionSort(worstCaseVector);
              } else if (algorithm == "selection") {
                selectionSort(worstCaseVector);
              } else if (algorithm == "quick") {
                quickSort(worstCaseVector, 0, worstCaseVector.size() - 1);
              }
              auto stop = high_resolution_clock::now();
              auto duration = duration_cast<microseconds>(stop - start);
              worstCase << algorithm << "," << size << "," << duration.count() << endl;

            vector<int> averageCaseVector = randomVector(size, 1, 10000);
              start = high_resolution_clock::now();
              if (algorithm == "bubble") { 
                bubbleSort(averageCaseVector);
              } else if (algorithm == "insertion") {
                insertionSort(averageCaseVector);
              } else if (algorithm == "selection") {
                selectionSort(averageCaseVector);
              }else if (algorithm == "quick") {
                quickSort(averageCaseVector, 0, averageCaseVector.size() - 1);
              }
              stop = high_resolution_clock::now();
              duration = duration_cast<microseconds>(stop - start);
              averageCase << algorithm << "," << size << "," << duration.count() << endl;

              vector<int> bestCaseVector = sortedVector(size);
              start = high_resolution_clock::now();
              if (algorithm == "bubble")  {
                bubbleSort(bestCaseVector);
              } else if (algorithm == "insertion") {
                insertionSort(bestCaseVector);
              } else if (algorithm == "selection") {
                selectionSort(bestCaseVector);
              } else if (algorithm == "quick") {
                quickSort(bestCaseVector, 0, bestCaseVector.size() - 1);
              }
              stop = high_resolution_clock::now();
              duration = duration_cast<microseconds>(stop - start);
              bestCase << algorithm << "," << size << "," << duration.count() << endl;
              durationTime.push_back(duration.count()); //Durationtime vector to get the output
          }
        double minTime = *min_element(durationTime.begin(), durationTime.end());
        double maxTime = *max_element(durationTime.begin(), durationTime.end());
        double meanTime = accumulate(durationTime.begin(), durationTime.end(), 0.0) / durationTime.size();
        double sdTime = sampleSD(durationTime);
        
          //exact output required from the instructions
          cout << "**********************\n";
          cout << algorithm << " sort on 50 vectors of length " << size << "\n";
          cout << "Sorting successful\n";
          cout << "Minimum: " << minTime << " sec; Mean: " << meanTime << " sec; Standard deviation: " << sdTime << " sec; Maximum: " << maxTime << " sec\n";
          cout << "**********************\n";
      }
  }
  
  //Instructors note: DO NOT forget to close the file below:
  worstCase.close();
  averageCase.close();
  bestCase.close();
    
  return 0; 
}
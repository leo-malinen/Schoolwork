#include "dstack.h"
#include <cmath>
#include <ctype.h>
#include <iostream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

stack<double> n;

double postFixes(char operators) {
  if (n.size() < 2) {
    return 1;
  } else {
    double op2 = n.top();
    n.pop();
    double op1 = n.top();
    n.pop();
    switch (operators) {
    case '+':
      n.push(op1 + op2);
      break;
    case '-':
      n.push(op1 - op2);
      break;
    case '*':
      n.push(op1 * op2);
      break;
    case '^':
      if (op2 == 0) {
        cout << "1" << endl;
        n.push(pow(op1, op2));
        return 2;
      } else if (op1 == 0 && op2 < 0) {
        return 1;
      } else {
        n.push(pow(op1, op2));
        break;
      }
    case '/':
      if (op2 == 0 && op1 != 0) {
        return 1;
      } else {
        n.push(op1 / op2);
        break;
      }
    default:
      return 1;
    }
    return n.top();
  }
}

int main() {
  string postFixExpression;
  // getline(cin, postFixExpression);
  // cout << evalPostFixExpression(postFixExpression) << endl;

  char char_variable;
  double double_variable;
  int returnStatus = 0;
  int operatorCounter = 0;
  int operandCounter = 0;
  // int dotSpaceCount = 0;

  while (cin.peek() != EOF) {
    if (isdigit(cin.peek())) {
      cin >> double_variable;
      n.push(double_variable);
      operandCounter++;
    } else if (cin.peek() == '+' || cin.peek() == '-' || cin.peek() == '*' ||
               cin.peek() == '/' || cin.peek() == '^') {
      cin >> char_variable;
      returnStatus = postFixes(char_variable);
      operatorCounter++;
    } else if (isspace(cin.peek())) {
      cin.ignore();
      continue;
    } else if (cin.peek() == '.') {
      // dotSpaceCount++;
      cin >> double_variable;
      n.push(double_variable);
      operandCounter++;
    }
  }

  if (operatorCounter - 1 >= operandCounter) {
    cerr << "Error: Invalid expression." << endl;
    return 1;
  }
  if (n.size() != 1) {
    cerr << "Error: Invalid expression." << endl;
    return 1;
  }
  if (returnStatus != 1 && returnStatus != 2) {
    cout << n.top() << endl;
  } else if (returnStatus == 1) {
    cerr << "Error: Invalid expression." << endl;
    return 1;
  }
}

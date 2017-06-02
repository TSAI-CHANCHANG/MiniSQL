#pragma once
#include <iostream>
#include <string>
using namespace std;

#define ERROR       -1
#define QUIT_NUMBER -2

#define SELECT       1
#define CREATE       2
#define DROP         3
#define INSERT       4
#define QUIT         5
#define EXEC         6
string getInput();

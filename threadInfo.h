#include <iostream>
#include "node.h"
using namespace std;

struct threadInfo
{
    FILE *numbersFile;
    int *numbersArray;
    node<int> *head;
    int start;
    int end;
};
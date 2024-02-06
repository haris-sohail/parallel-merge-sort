#include <iostream>
#include "node.h"
using namespace std;

struct threadInfo
{
    FILE *numbersFile;
    node<int> *head;
};
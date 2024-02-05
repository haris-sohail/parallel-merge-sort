#include <iostream>
#include"node.h"
#include"util.h"
using namespace std;

int main()
{
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    int *numbers = new int[1000];
    int num;

    readRollNumbers(numbersFile, numbers, num);

    node<int> *head = new node<int>;

    addRollNumbersToList(head, numbers, num);
}
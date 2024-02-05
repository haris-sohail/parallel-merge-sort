#include <iostream>
#include <fstream>
#include <string>
#include "node.h"
using namespace std;

size_t getArraySize(const int *arr)
{
    // Assuming the array is terminated by a special value, like -1
    size_t size = 0;
    while (arr[size] != -1)
    {
        ++size;
    }
    return size;
}

void printIntArray(const int *arr, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        cout << arr[i] << std::endl;
    }
}

void readRollNumbers(FILE *inputFile, int *Numbers, int num)
{
    char number[256];
    int numbersI = 0;
    string number_in_String;
    int numberI;

    // read the file and add numbers to the array
    while (fgets(number, sizeof(inputFile), inputFile) != nullptr)
    {
        number_in_String = number;
        numberI = stoi(number_in_String);
        Numbers[numbersI++] = numberI;
    }
    Numbers[numbersI] = -1;
}

void insertInList(node<int> *head, int value)
{
    node<int> *newNode = new node<int>(value);
    newNode->next = NULL;

    node<int> *currentNode = head;
    while (currentNode->next != NULL)
    {
        currentNode = currentNode->next;
    }

    currentNode->next = newNode;
}

void addRollNumbersToList(node<int> *head, int *Numbers, int num)
{
    // insert first element at head
    head->val = Numbers[0];
    int arraySize = getArraySize(Numbers);

    // insert remaining in the next of each node
    for (int i = 1; i < arraySize; i++)
    {
        insertInList(head, Numbers[i]);
    }
}
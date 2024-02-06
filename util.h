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
        cout << arr[i] << endl;
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

node<int> *findMid(node<int> *head)
{
    node<int> *slow = head;
    node<int> *fast = head->next;

    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = (fast->next)->next;
    }

    // slow is pointing at the middle of the list
    return slow;
}

node<int> *merge(node<int> *left, node<int> *right)
{
    node<int> *dummyHead = new node<int>();
    node<int> *current = dummyHead;

    while (left != NULL && right != NULL)
    {
        if (left->val <= right->val)
        {
            current->next = left;
            left = left->next;
            current = current->next;
        }
        else if (right->val < left->val)
        {
            current->next = right;
            right = right->next;
            current = current->next;
        }
    }
    while (left != NULL)
    {
        current->next = left;
        left = left->next;
        current = current->next;
    }
    while (right != NULL)
    {
        current->next = right;
        right = right->next;
        current = current->next;
    }
    return dummyHead->next;
}

void printLinkedList(node<int> *head)
{
    while (head != nullptr)
    {
        cout << head->val << " ";
        head = head->next;
    }
    cout << endl;
}

void writeResultToFile(node<int> *head, const string &filename)
{
    ofstream outFile(filename);

    if (!outFile.is_open())
    {
        cerr << "Error: Couldn't open the file " << filename << " for writing.\n";
        return;
    }

    node<int> *curr = head;

    while (curr != nullptr)
    {
        outFile << curr->val << "\n";
        curr = curr->next;
    }

    outFile.close();

    cout << "Results written to file successfully" << endl;
}

node<int> *mergeSort(node<int> *head)
{
    if (head->next == NULL)
    {
        return head;
    }

    node<int> *mid = findMid(head);
    node<int> *startOfRight = mid->next;

    mid->next = NULL;

    node<int> *left = mergeSort(head);
    node<int> *right = mergeSort(startOfRight);

    node<int> *newHead = merge(left, right);

    return newHead;
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

void writeNumbersToFile(const string &filename)
{
    ofstream outFile(filename);

    if (!outFile.is_open())
    {
        cerr << "Error: Couldn't open the file " << filename << " for writing.\n";
        return;
    }

    // Write 100000 random numbers separated by newlines
    for (int i = 0; i < 1000; ++i)
    {
        outFile << rand() % 10000 << '\n';
    }

    outFile.close();
}
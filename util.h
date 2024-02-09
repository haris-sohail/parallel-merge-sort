#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include "node.h"
#include <mutex>
#include "threadInfo.h"
using namespace std;

mutex listMutex;

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

void executeSerial()
{
    writeNumbersToFile("./assets/numbers.txt");
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    int *numbers = new int[100000];
    int num;

    readRollNumbers(numbersFile, numbers, num);

    node<int> *head = new node<int>;

    addRollNumbersToList(head, numbers, num);

    delete[] numbers;

    // head = mergeSort(head);

    // printLinkedList(head);

    writeResultToFile(head, "./assets/result.txt");
}

void *addRollNumbersToListParallel(void *arg)
{
    threadInfo *threadInfoObj = (threadInfo *)arg;
    int start = threadInfoObj->start;
    int end = threadInfoObj->end;
    node<int> *head = threadInfoObj->head;
    int *Numbers = threadInfoObj->numbersArray;

    for (int i = start; i < end; i++)
    {
        insertInList(head, Numbers[i]);
    }
}

void setAffinity(pthread_t thread, int coreId)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);

    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0)
    {
        perror("pthread_setaffinity_np");
    }
}

void setThreadInfo(threadInfo *&threadInfoArray, int *Numbers)
{
    // set head and numbers file
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    node<int> **heads = new node<int> *[4];

    // Initialize each head node
    for (int i = 0; i < 4; ++i)
    {
        heads[i] = new node<int>;
    }

    for (int i = 0; i < 4; i++)
    {
        threadInfoArray[i].head = heads[i];
        threadInfoArray[i].numbersFile = numbersFile;
        threadInfoArray[i].numbersArray = Numbers;
    }

    int arraySize = getArraySize(Numbers);
    int portion = arraySize / 4;

    // divide the numbers array into four portions using start and end portion
    for (int i = 0; i < 4; i++)
    {
        threadInfoArray[i].start = i * portion;
        threadInfoArray[i].end = (i == 4 - 1) ? arraySize : (i + 1) * portion;
    }
}

node<int>* mergeLists(node<int>* list1, node<int>* list2) {
    if (list1 == nullptr) return list2;
    if (list2 == nullptr) return list1;

    node<int>* mergedHead = nullptr;
    if (list1->val <= list2->val) {
        mergedHead = list1;
        mergedHead->next = mergeLists(list1->next, list2);
    } else {
        mergedHead = list2;
        mergedHead->next = mergeLists(list1, list2->next);
    }
    return mergedHead;
}

void createThreads(node<int> *head, FILE *numbersFile, int *Numbers)
{
    threadInfo *threadInfoArray = new threadInfo[4];

    pthread_t input[4];

    pthread_attr_t atr;

    pthread_attr_init(&atr);

    // making thread joinable
    pthread_attr_setdetachstate(&atr, PTHREAD_CREATE_JOINABLE);

    setThreadInfo(threadInfoArray, Numbers);

    for (int i = 0; i <= 4; i++)
    {
        pthread_create(&input[i], &atr, addRollNumbersToListParallel, &threadInfoArray[i]);
        setAffinity(input[i], i);
    }

    for (int i = 0; i <= 4; i++)
    {
        pthread_join(input[i], NULL);
    }

    // join all linked lists to make one
    for (int i = 0; i < 4; i++){
        mergeLists(head, threadInfoArray[i].head);
    }
}

void removeAtHead(node<int> *&head)
{
    node<int> *temp = head;
    head = head->next;

    delete temp;
}

void* mergeSortParallel(void* arg){
    
}

void executeParallel()
{
    int *numbers = new int[100000];
    int num;
    node<int> *head = new node<int>;
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    writeNumbersToFile("./assets/numbers.txt");

    readRollNumbers(numbersFile, numbers, num);

    createThreads(head, numbersFile, numbers);

    // head contains redundant zeros
    removeAtHead(head); 
    removeAtHead(head); 
    removeAtHead(head); 
    removeAtHead(head); 
    removeAtHead(head); 
    
    writeResultToFile(head, "./assets/result.txt");
}

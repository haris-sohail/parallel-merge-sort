#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <pthread.h>
#include "node.h"
#include <mutex>
#include "threadInfo.h"
#include "mergeInfo.h"
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

void writeNumbersToFile(const string &filename, int numbersSize)
{
    ofstream outFile(filename);

    if (!outFile.is_open())
    {
        cerr << "Error: Couldn't open the file " << filename << " for writing.\n";
        return;
    }

    // Write random numbers separated by newlines
    for (int i = 0; i < numbersSize; ++i)
    {
        outFile << rand() % 10000 << '\n';
    }

    outFile.close();
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

node<int> *mergeLists(node<int> *list1, node<int> *list2)
{
    if (list1 == nullptr)
        return list2;
    if (list2 == nullptr)
        return list1;

    node<int> *mergedHead = nullptr;
    if (list1->val <= list2->val)
    {
        mergedHead = list1;
        mergedHead->next = mergeLists(list1->next, list2);
    }
    else
    {
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

    for (int i = 0; i < 4; i++)
    {
        pthread_create(&input[i], &atr, addRollNumbersToListParallel, &threadInfoArray[i]);
        setAffinity(input[i], i);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(input[i], NULL);
    }

    // join all linked lists to make one
    for (int i = 0; i < 4; i++)
    {
        mergeLists(head, threadInfoArray[i].head);
    }
}

void removeAtHead(node<int> *&head)
{
    node<int> *temp = head;
    head = head->next;

    delete temp;
}

void divideIntoEight(node<int> *head, node<int> *&head1, node<int> *&head2, node<int> *&head3, node<int> *&head4,
                     node<int> *&head5, node<int> *&head6, node<int> *&head7, node<int> *&head8)
{
    if (head == nullptr)
    {
        head1 = head2 = head3 = head4 = head5 = head6 = head7 = head8 = nullptr;
        return;
    }

    int length = 0;
    node<int> *current = head;

    // Calculate the length of the linked list
    while (current != nullptr)
    {
        length++;
        current = current->next;
    }

    // Calculate the size of each portion
    int portionSize = length / 8;
    int remainder = length % 8;

    current = head;
    head1 = current;

    // Move to the end of the first portion
    for (int i = 1; i < portionSize; ++i)
    {
        current = current->next;
    }

    // Split the first portion and update head2
    head2 = current->next;
    current->next = nullptr;
    current = head2;

    // Move to the end of the second portion
    for (int i = 1; i < portionSize + (remainder > 0 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the second portion and update head3
    head3 = current->next;
    current->next = nullptr;
    current = head3;

    // Move to the end of the third portion
    for (int i = 1; i < portionSize + (remainder > 1 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the third portion and update head4
    head4 = current->next;
    current->next = nullptr;
    current = head4;

    // Move to the end of the fourth portion
    for (int i = 1; i < portionSize + (remainder > 2 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the fourth portion and update head5
    head5 = current->next;
    current->next = nullptr;
    current = head5;

    // Move to the end of the fifth portion
    for (int i = 1; i < portionSize + (remainder > 3 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the fifth portion and update head6
    head6 = current->next;
    current->next = nullptr;
    current = head6;

    // Move to the end of the sixth portion
    for (int i = 1; i < portionSize + (remainder > 4 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the sixth portion and update head7
    head7 = current->next;
    current->next = nullptr;
    current = head7;

    // Move to the end of the seventh portion
    for (int i = 1; i < portionSize + (remainder > 5 ? 1 : 0); ++i)
    {
        current = current->next;
    }

    // Split the seventh portion and update head8
    head8 = current->next;
    current->next = nullptr;
}

void setMergeInfo(mergeInfo *&mergeInfoArray, node<int> *head1, node<int> *head2,
                  node<int> *head3, node<int> *head4, node<int> *head5,
                  node<int> *head6, node<int> *head7, node<int> *head8)
{
    mergeInfoArray[0].left = head1;
    mergeInfoArray[0].right = head2;

    mergeInfoArray[1].left = head3;
    mergeInfoArray[1].right = head4;

    mergeInfoArray[2].left = head5;
    mergeInfoArray[2].right = head6;

    mergeInfoArray[3].left = head7;
    mergeInfoArray[3].right = head8;
}

void setMergeInfo_1(mergeInfo *&mergeInfoArray, node<int> *head1, node<int> *head2,
                    node<int> *head3, node<int> *head4)
{
    mergeInfoArray[0].left = head1;
    mergeInfoArray[0].right = head2;

    mergeInfoArray[1].left = head3;
    mergeInfoArray[1].right = head4;
}

node<int> *sort(node<int> *left, node<int> *right)
{
    node<int> *dummyHead = new node<int>();
    node<int> *current = dummyHead;

    while (left != nullptr && right != nullptr)
    {
        if (left->val <= right->val)
        {
            current->next = left;
            left = left->next;
        }
        else
        {
            current->next = right;
            right = right->next;
        }
        current = current->next;
    }

    // Append remaining elements if any
    if (left != nullptr)
    {
        current->next = left;
    }
    else
    {
        current->next = right;
    }

    // Return the head of the merged sorted list
    node<int> *sortedList = dummyHead->next;
    delete dummyHead; // Free the dummy head node
    return sortedList;
}

void *mergeSortParallelThread(void *arg)
{
    mergeInfo *mergeInfoObj = (mergeInfo *)arg;

    node<int> *left = mergeInfoObj->left;

    node<int> *right = mergeInfoObj->right;

    // sort left and right and then merge them

    left = mergeSort(left);
    right = mergeSort(right);

    node<int> *newHead = merge(left, right);

    mergeInfoObj->result = newHead;
}

void createThreadsSort(mergeInfo *&mergeInfoArray, node<int> *head, node<int> *head1, node<int> *head2, node<int> *head3, node<int> *head4,
                       node<int> *head5, node<int> *head6, node<int> *head7, node<int> *head8)
{
    pthread_t merge[4];

    pthread_attr_t atr;

    pthread_attr_init(&atr);

    // making thread joinable
    pthread_attr_setdetachstate(&atr, PTHREAD_CREATE_JOINABLE);

    setMergeInfo(mergeInfoArray, head1, head2, head3, head4, head5, head6, head7, head8);

    for (int i = 0; i < 4; i++)
    {
        pthread_create(&merge[i], &atr, mergeSortParallelThread, &mergeInfoArray[i]);
        setAffinity(merge[i], i);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(merge[i], NULL);
    }
}

void createThreadsSort_1(mergeInfo *&mergeInfoArray, node<int> *head1, node<int> *head2,
                         node<int> *head3, node<int> *head4)
{
    pthread_t merge[4];

    pthread_attr_t atr;

    pthread_attr_init(&atr);

    // making thread joinable
    pthread_attr_setdetachstate(&atr, PTHREAD_CREATE_JOINABLE);

    setMergeInfo_1(mergeInfoArray, head1, head2, head3, head4);

    for (int i = 0; i < 2; i++)
    {
        pthread_create(&merge[i], &atr, mergeSortParallelThread, &mergeInfoArray[i]);
        setAffinity(merge[i], i);
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_join(merge[i], NULL);
    }
}

node<int> *mergeSortParallel(node<int> *head)
{
    mergeInfo *mergeInfoArray = new mergeInfo[4];
    mergeInfo *mergeInfoArray_1 = new mergeInfo[2];

    if (head->next == NULL)
    {
        return head;
    }

    // divide list into eight parts
    node<int> *head1 = new node<int>;
    node<int> *head2 = new node<int>;
    node<int> *head3 = new node<int>;
    node<int> *head4 = new node<int>;
    node<int> *head5 = new node<int>;
    node<int> *head6 = new node<int>;
    node<int> *head7 = new node<int>;
    node<int> *head8 = new node<int>;

    divideIntoEight(head, head1, head2, head3, head4, head5, head6, head7, head8);

    // combine 8 lists into 4 sorted lists

    createThreadsSort(mergeInfoArray, head, head1, head2, head3, head4, head5, head6, head7, head8);

    // combine 4 lists into 2 sorted lists

    createThreadsSort_1(mergeInfoArray_1, mergeInfoArray[0].result, mergeInfoArray[1].result,
                        mergeInfoArray[2].result, mergeInfoArray[3].result);

    // combine 2 lists into a single list

    node<int> *newHead = merge(mergeInfoArray_1[0].result, mergeInfoArray_1[1].result);

    return newHead;
}

auto measureStartTime()
{
    // Measure the start time
    auto startTime = chrono::high_resolution_clock::now();

    return startTime;
}

void executeParallel()
{
    auto startTime = measureStartTime();

    int *numbers = new int[100000];
    int num;
    node<int> *head = new node<int>;
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    writeNumbersToFile("./assets/numbers.txt", 100000);

    readRollNumbers(numbersFile, numbers, num);

    createThreads(head, numbersFile, numbers);

    // head contains redundant zeros
    removeAtHead(head);
    removeAtHead(head);
    removeAtHead(head);
    removeAtHead(head);
    removeAtHead(head);

    head = mergeSortParallel(head);

    // printLinkedList(head);

    writeResultToFile(head, "./assets/result.txt");

    // Measure the end time
    auto endTime = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    cout << "Parallel Merge Sort Execution Time: " << duration.count() << " milliseconds" << endl;
}

void executeSerial()
{
    auto startTime = measureStartTime();

    writeNumbersToFile("./assets/numbers.txt", 100000);
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    int *numbers = new int[100000];
    int num;

    readRollNumbers(numbersFile, numbers, num);

    node<int> *head = new node<int>;

    addRollNumbersToList(head, numbers, num);

    delete[] numbers;

    head = mergeSort(head);

    // printLinkedList(head);

    writeResultToFile(head, "./assets/result.txt");

    // Measure the end time
    auto endTime = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    cout << "Serial Merge Sort Execution Time: " << duration.count() << " milliseconds" << endl;
}

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

size_t getArraySize(const int* arr) {
    // Assuming the array is terminated by a special value, like -1
    size_t size = 0;
    while (arr[size] != -1) {
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

    printIntArray(Numbers, getArraySize(Numbers));
}

int main()
{
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    int *numbers = new int[1000];
    int num;

    readRollNumbers(numbersFile, numbers, num);
}
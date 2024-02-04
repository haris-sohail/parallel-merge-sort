#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void readRollNumbers(FILE *inputFile, int *Numbers, int num)
{
    char number[256];

    while (fgets(number, sizeof(inputFile), inputFile) != nullptr) {
        cout << number;
    }
}

int main()
{
    FILE *numbersFile = fopen("./assets/numbers.txt", "r");

    int *numbers;
    int num;

    readRollNumbers(numbersFile, numbers, num);
}
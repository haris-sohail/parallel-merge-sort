#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void readRollNumbers(FILE *inputFile, int *Numbers, int num)
{
}

int main()
{
    string number;

    ifstream numbers("./assets/numbers.txt");

    while (getline(numbers, number)){
        cout << number << endl;
    }

    numbers.close();
}
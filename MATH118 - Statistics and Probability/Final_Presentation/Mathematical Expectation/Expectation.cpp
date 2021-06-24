#include <iostream>
#include "Expectation.hpp"
#include <vector>
#include <time.h>  

using namespace std;

Expectation::Expectation()
{
    /* Initially Empty */
}

void Expectation::exampleDice(int howManyTimes)
{
    // generate new seed
    srand(time(NULL));

    int sum = 0;
    vector<int> freq = generateFreq(6);

    for (int j = 0; j < howManyTimes; j++)
    {
        int randVal = rand() % 6 + 1;
        freq[randVal - 1] += 1;
        switch (randVal)
        {
        case 2:
            sum += 20;
            break;
        case 4:
            sum += 40;
            break;
        case 6:
            sum -= 30;
            break;
        default:
            break;
        }
    }

    printVector(freq);
    cout << "Result:" <<  sum / howManyTimes << endl;
    return sum / howManyTimes;
}   

vector<int> Expectation::generateFreq(int num)
{
    vector<int> freq;
    for (int i = 0; i < num; i++)
        freq.push_back(0);

    return freq;
}

void Expectation::printVector(vector<int> vec)
{
    cout << " ------- Frequency Table -------" << endl;
    cout << "["; 
    for (int i = 0; i < vec.size(); i++)
    {
        cout << i+1 << ":" << vec[i];
        if (i != vec.size() - 1)
            cout << ", ";
    }
    cout << "]" << endl;
}


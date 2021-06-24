#include <iostream>
#include <vector>
#include <map>

#pragma once

class Expectation
{
public:
    Expectation(/* args */);
    
    void exampleDice(int howManyTimes);
    void exampleAppointment(int howManyTimes);
    void varianceFinder(std::map<double,double> values, double mean);
    double meanFinder(std::map<double,double> m);

private:
    /* data */
    std::vector<int> freq_dice;

    /* private functions */
    std::vector<int> generateFreq(int size);
    void printVector(std::vector<int> vec);
};



#include "aoc5b.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "../aoc_5a/input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int pairTarget{1};
    int repeatTarget{1};
    int niceCount {aoc5b::countNiceStrings(input, pairTarget, repeatTarget)};
    std::cout << "The number of nice strings is: " << niceCount << '\n';
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

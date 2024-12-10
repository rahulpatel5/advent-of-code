#include "aoc5a.h"
#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int vowelTarget{3};
    int doubleTarget{1};
    int niceCount {aoc5a::countNiceStrings(input, vowelTarget, doubleTarget)};
    std::cout << "The number of nice strings is: " << niceCount << '\n';
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

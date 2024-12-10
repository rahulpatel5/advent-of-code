#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include "aoc2b.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "../aoc_2a/input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int area{ aoc2b::processLines(input) };
    std::cout << "Need " << area << " square feet of ribbon.\n";

    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

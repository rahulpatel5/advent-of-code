#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include "aoc2a.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int area{ aoc2a::processLines(input) };
    std::cout << "Need " << area << " square feet of wrapping paper.\n";

    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

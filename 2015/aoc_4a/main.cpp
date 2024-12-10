#include "aoc4a.h"
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
    std::string input { utils::ReadFile(file) };

    int targetZeroes {5};
    int maxIterations {10'000'000};
    aoc4a::getFirstValidKey(input, targetZeroes, maxIterations);
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

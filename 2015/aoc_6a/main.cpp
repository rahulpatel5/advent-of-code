#include "aoc6a.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

#include <vector>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::ifstream file {"input.txt"};
    
    utils::CheckFileError(file);
    const std::string input {utils::ReadFile(file)};

    // BitMatrix has a fixed 1,000 columns
    constexpr int rows{1'000};
    int lightsOn {aoc6a::runLightInstructions(input, rows)};
    std::cout << "The number of activated lights are: " << lightsOn << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

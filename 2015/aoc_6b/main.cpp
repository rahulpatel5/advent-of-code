#include "aoc6b.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

#include <vector>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::ifstream file {"../aoc_6a/input.txt"};
    
    utils::CheckFileError(file);
    const std::string input {utils::ReadFile(file)};

    constexpr int rows{1'000};
    constexpr int columns{1'000};
    int lightsOn {aoc6b::runLightInstructions(input, rows, columns)};
    std::cout << "The number of activated lights are: " << lightsOn << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

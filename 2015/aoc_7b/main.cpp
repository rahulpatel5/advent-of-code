#include "aoc7b.h"
#include "../aoc_7a/bitwise.h"
#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::ifstream file {"../aoc_7a/input.txt"};
    
    utils::CheckFileError(file);
    const std::string input {utils::ReadFile(file)};

    bitwise::StringIntMap circuit {aoc7b::parseAndResetWires(input)};
    std::cout << "The signal for wire a is: " << circuit["a"] << '\n';

    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

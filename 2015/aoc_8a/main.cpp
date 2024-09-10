#include "aoc8a.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::ifstream file {"input.txt"};
    
    utils::CheckFileError(file);
    const std::string input {utils::ReadFile(file)};
    // std::string test {"\"\"\n\"abc\"\n\"aaa\\\"aaa\"\n\"\\x27\""};
    
    int letterDifference {aoc8a::parseStrings(input)};
    std::cout << "Number of escape characters = " << letterDifference << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

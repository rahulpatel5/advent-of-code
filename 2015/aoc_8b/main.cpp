#include "aoc8b.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::ifstream file {"../aoc_8a/input.txt"};
    
    utils::CheckFileError(file);
    const std::string input {utils::ReadFile(file)};
    // std::string test {"\"\"\n\"abc\"\n\"aaa\\\"aaa\"\n\"\\x27\""};
    
    int newEscapeChars {aoc8b::parseStrings(input)};
    std::cout << "Number of added escape characters = " << newEscapeChars << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

#include "aoc3b.h"
#include "../aoc_3a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>
#include <string>
#include <algorithm>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    std::string input {input::input};
    // combine all lines into one input
    // we need continuous text to handle start and stop points
    input.erase(std::remove(input.begin(), input.end(), '\n'), input.cend());
    
    std::string testInput {R"(xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5)))"};
    
    int result {aoc3b::parseAndMultiply(input)};
    std::cout << "Final result: " << result << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

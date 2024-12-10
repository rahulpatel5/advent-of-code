#include "aoc2b.h"
#include "../aoc_2a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr auto lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr auto testLines {StringUtils::split<testLength>(testInput)};
    
    int safeCount {aoc2b::parseInputAndCountSafe(lines)};
    std::cout << "Number of safe reports: " << safeCount << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

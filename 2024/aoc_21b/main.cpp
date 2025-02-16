#include "aoc21b.h"
#include "../aoc_21a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(029A
980A
179A
456A
379A
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};

    constexpr int extraDirectionalRobots {25};
    // aoc21b::CodeInt complexity {aoc21b::parseAndGetCodeComplexity<testLength, extraDirectionalRobots>(testLines)};
    aoc21b::CodeInt complexity {aoc21b::parseAndGetCodeComplexity<length, extraDirectionalRobots>(lines)};
    std::cout << "sum of complexities: " << complexity << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

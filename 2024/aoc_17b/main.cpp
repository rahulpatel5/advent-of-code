#include "aoc17b.h"
#include "../aoc_17a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(Register A: 2024
Register B: 0
Register C: 0

Program: 0,3,5,4,3,0
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc17b::RegisterInt output {aoc17b::parseAndGetCopyProgram<testLength>(testLines)};
    aoc17b::RegisterInt output {aoc17b::parseAndGetCopyProgram<length>(lines)};
    std::cout << "Smallest register A value for copy program: " << output << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

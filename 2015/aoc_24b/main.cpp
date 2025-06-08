#include "aoc24b.h"
#include "../aoc_24a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(1
2
3
4
5
7
8
9
10
11
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    constexpr int numberGroups {4};
    aoc24b::Quantum quantum {aoc24b::getPresentEntanglement<length, numberGroups>(lines)};
    std::cout << "Quantum entanglement: " << quantum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

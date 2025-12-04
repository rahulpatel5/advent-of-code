#include "aoc3b.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(987654321111111
811111111111119
234234234234278
818181911112111
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc3b::JoltInt joltage { aoc3b::getTotalJoltage<testLength>(testLines) };
    aoc3b::JoltInt joltage { aoc3b::getTotalJoltage<length>(lines) };
    std::cout << "total joltage: " << joltage << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

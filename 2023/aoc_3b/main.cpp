#include "aoc3b.h"
#include "../aoc_3a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc3b::GearInt sum {aoc3b::getSumOfGearNumbers<testLength>(testLines)};
    aoc3b::GearInt sum {aoc3b::getSumOfGearNumbers<length>(lines)};
    std::cout << "Sum of gear numbers is: " << sum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

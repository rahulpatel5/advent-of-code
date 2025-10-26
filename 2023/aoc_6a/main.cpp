#include "aoc6a.h"
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
    
    constexpr std::string_view testInput {R"(Time:      7  15   30
Distance:  9  40  200
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc6a::TimeInt time {aoc6a::getTravelTime<testLength>(testLines)};
    aoc6a::TimeInt time {aoc6a::getTravelTime<length>(lines)};
    std::cout << "Time is: " << time << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

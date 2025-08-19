#include "aoc1a.h"
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
    
    constexpr std::string_view testInput {R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc1a::CalInt sum {aoc1a::getCalibrationSum<testLength>(testLines)};
    aoc1a::CalInt sum {aoc1a::getCalibrationSum<length>(lines)};
    std::cout << "Sum of calibration values is: " << sum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

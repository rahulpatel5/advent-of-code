#include "aoc6b.h"
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
    
    constexpr std::string_view testInput {R"(123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc6b::MathsInt total { aoc6b::doCephalopodMaths<testLength>(testLines) };
    aoc6b::MathsInt total { aoc6b::doCephalopodMaths<length>(lines) };
    std::cout << "grand total: " << total << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

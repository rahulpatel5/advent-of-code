#include "aoc17a.h"
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
    
    constexpr int target {150};
    aoc17a::ComboInt combos {aoc17a::countCombos<length, target>(lines)};
    std::cout << "Number of combinations of containers: " << combos << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

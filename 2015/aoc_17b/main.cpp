#include "aoc17b.h"
#include "../aoc_17a/input.h"
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
    aoc17b::ComboInt minCombo {aoc17b::getMinCombo<length, target>(lines)};
    std::cout << "Minimum number of containers: " << minCombo << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

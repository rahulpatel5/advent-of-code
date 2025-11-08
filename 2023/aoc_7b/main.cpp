#include "aoc7b.h"
#include "../aoc_7a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc7b::WinningInt wins {aoc7b::getTotalWinnings<testLength>(testLines)};
    aoc7b::WinningInt wins {aoc7b::getTotalWinnings<length>(lines)};
    std::cout << "total winnings: " << wins << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

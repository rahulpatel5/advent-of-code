#include "aoc22b.h"
#include "../aoc_22a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view boss {input::input};
    constexpr std::size_t bossLength {StringUtils::length(boss)};
    constexpr std::array<std::string_view, bossLength> bossInfo {StringUtils::split<bossLength>(boss)};
    
    aoc22b::Mana mana {aoc22b::getLeastManaToWin<bossLength>(bossInfo)};
    std::cout << "The least mana to win: " << mana << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

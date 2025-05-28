#include "aoc21a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view boss {input::input};
    constexpr std::size_t bossLength {StringUtils::length(boss)};
    constexpr std::array<std::string_view, bossLength> bossInfo {StringUtils::split<bossLength>(boss)};
    
    constexpr std::string_view shop {R"(Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3
)"};
    constexpr std::size_t shopLength {StringUtils::length(shop)};
    constexpr std::array<std::string_view, shopLength> itemShop {StringUtils::split<shopLength>(shop)};
    
    aoc21a::GoldInt gold {aoc21a::getLeastGoldToWin<bossLength, shopLength>(bossInfo, itemShop)};
    std::cout << "The least gold to win: " << gold << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

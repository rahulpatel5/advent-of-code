#include "aoc20b.h"
#include "../aoc_20a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view targetStr {input::input};
    
    aoc20b::HouseInt houseNo {aoc20b::getLowestHouseNumber(targetStr)};
    std::cout << "Lowest house number to get target presents: " << houseNo << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

#include "aoc20a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>
#include <string>

int main()
{
    Timer timer {};
    
    constexpr std::string_view targetStr {input::input};
    
    aoc20a::HouseInt houseNo {aoc20a::getLowestHouseNumber(targetStr)};
    std::cout << "Lowest house number to get target presents: " << houseNo << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

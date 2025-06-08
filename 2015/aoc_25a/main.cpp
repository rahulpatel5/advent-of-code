#include "aoc25a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    
    constexpr aoc25a::CodeInt firstCode {20'151'125};
    constexpr aoc25a::CodeInt multiplier {252'533};
    constexpr aoc25a::CodeInt divisor {33'554'393};
    aoc25a::CodeInt code {aoc25a::getCode(input, firstCode, multiplier, divisor)};
    std::cout << "The code is: " << code << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

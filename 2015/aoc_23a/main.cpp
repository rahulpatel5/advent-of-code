#include "aoc23a.h"
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
    
    aoc23a::Register registerB {aoc23a::getFinalRegister<length>(lines)};
    std::cout << "The final value in register b: " << registerB << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

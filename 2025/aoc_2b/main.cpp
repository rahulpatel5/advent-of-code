#include "aoc2b.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    
    constexpr std::string_view testInput {R"(11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124)"};
    
    // aoc2b::InvalidInt sum { aoc2b::sumInvalidIDs(testInput) };
    aoc2b::InvalidInt sum { aoc2b::sumInvalidIDs(input) };
    std::cout << "sum of invalid IDs: " << sum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

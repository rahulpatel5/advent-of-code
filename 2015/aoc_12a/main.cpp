#include "aoc12a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    
    constexpr std::string_view testInput {R"({"a":{"b":4},"c":-1})"};
    
    aoc12a::SumInt sum {aoc12a::jsonSum(input)};
    std::cout << "sum of numbers: " << sum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

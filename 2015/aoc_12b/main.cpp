#include "aoc12b.h"
#include "../aoc_12a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    
    constexpr std::string_view testInput {R"({"d":"red","e":[1,2,3,4],"f":5})"};
    
    aoc12b::SumInt sum {aoc12b::jsonSum(input)};
    std::cout << "sum of numbers: " << sum << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

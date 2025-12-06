#include "aoc5b.h"
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
    
    constexpr std::string_view testInput {R"(3-5
10-14
16-20
12-18

1
5
8
11
17
32
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    // aoc5b::IngredientInt ingredients { aoc5b::countAllIngredients<testLength>(testLines) };
    aoc5b::IngredientInt ingredients { aoc5b::countAllIngredients<length>(lines) };
    std::cout << "number of ingredients: " << ingredients << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

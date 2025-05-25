#include "aoc18b.h"
#include "../aoc_18a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    constexpr int steps {100};
    
    constexpr std::string_view testInput {R"(.#.#.#
...##.
#....#
..#...
#.#..#
####..
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    constexpr int testSteps {5};
    
    aoc18b::LightInt lights {aoc18b::countOnLights<length, steps>(lines)};
    std::cout << "Number of lights switched on: " << lights << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

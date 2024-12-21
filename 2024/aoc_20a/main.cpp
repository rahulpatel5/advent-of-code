#include "aoc20a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    constexpr int testTarget {10}; // test saving 10 ps
    
    constexpr std::string_view testInput {R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    constexpr int target {100};
    
    int saved {aoc20a::parseAndCountSavingCheats<length, target>(lines)};
    std::cout << "cheats saving at least the target time: " << saved << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

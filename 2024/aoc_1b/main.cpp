#include "aoc1b.h"
#include "../aoc_1a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr auto lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(3   4
4   3
2   5
1   3
3   9
3   3
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr auto testLines {StringUtils::split<testLength>(testInput)};
    
    int similarityScore {aoc1b::parseAndGetSimilarityScore<std::array<std::string_view, length>, length>(lines)};
    std::cout << "Similarity score: " << similarityScore << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

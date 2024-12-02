#include "aoc10a.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>
#include <vector>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {R"(1113222113)"};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr auto lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(1)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr auto testLines {StringUtils::split<testLength>(testInput)};
    
    constexpr int iterations {40};
    std::vector<int> result {aoc10a::playLookAndSay(input, iterations)};
    std::cout << "length of result: " << result.size() << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

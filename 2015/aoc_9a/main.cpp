#include "aoc9a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr auto lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view test {"London to Dublin = 464\nLondon to Belfast = 518\nDublin to Belfast = 141\n"};
    constexpr std::size_t testLength {StringUtils::length(test)};
    constexpr auto testLines {StringUtils::split<testLength>(test)};
    
    int route {aoc9a::getShortestRoute<std::array<std::string_view, length>>(lines)};
    std::cout << "The distance for the shortest route is: " << route << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

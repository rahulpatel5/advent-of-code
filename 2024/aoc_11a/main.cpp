#include "aoc11a.h"
#include "input.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::string_view testInput {"125 17"};
    constexpr int blinks {25};
    
    int stones {aoc11a::parseAndCountStones(input, blinks)};
    std::cout << "number of stones: " << stones << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

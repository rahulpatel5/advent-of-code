#include "aoc9b.h"
#include "../aoc_9a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    
    constexpr std::string_view testInput {R"(2333133121414131402)"};
    
    long long checksum {aoc9b::parseAndGetChecksum(input)};
    std::cout << "checksum is: " << checksum << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

#include "aoc18b.h"
#include "../aoc_18a/input.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    constexpr size_t grid {71}; // puzzle fits in coordinates of [0, 70]
    constexpr int bytes {1'024};
    
    constexpr std::string_view testInput {R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    constexpr size_t testGrid {7}; // test grid fits in [0, 6]
    constexpr int testBytes {12};
    
    std::pair<size_t, size_t> blocker {aoc18b::parseAndGetFirstExitBlocker<length, grid, bytes>(lines)};
    std::cout << "first byte preventing exit: " << blocker.first << ',' << blocker.second << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

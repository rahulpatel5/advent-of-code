#include "aoc10b.h"
#include "../../shared/StringUtils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {R"(1113222113)"};
    
    constexpr int iterations {50};
    std::vector<int> result {aoc10b::playLookAndSay(input, iterations)};
    std::cout << "length of result: " << result.size() << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

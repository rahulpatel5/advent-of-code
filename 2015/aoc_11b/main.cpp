#include "../aoc_11a/aoc11a.h"
#include <chrono>
#include <iostream>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {R"(vzbxkghb)"};
    
    std::string nextPassword {aoc11a::parseAndGetNextPassword(input)};
    nextPassword = aoc11a::incrementPassword(nextPassword);
    std::string yetAnotherPassword {aoc11a::parseAndGetNextPassword(nextPassword)};
    std::cout << "next password is: " << yetAnotherPassword << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

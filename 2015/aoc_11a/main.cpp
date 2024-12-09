#include "aoc11a.h"
#include "input.h"
#include <chrono>
#include <iostream>
#include <string>

int main()
{
    auto startTime {std::chrono::high_resolution_clock::now()};
    
    constexpr std::string_view input {input::input};
    
    constexpr std::string_view testInput1 {R"(abcdefgh)"};
    constexpr std::string_view testInput2 {R"(ghjaabcc)"};
    
    std::string nextPassword {aoc11a::parseAndGetNextPassword(input)};
    std::cout << "next password is: " << nextPassword << '\n';
    
    auto stopTime {std::chrono::high_resolution_clock::now()};
    const std::chrono::duration<double> durationSeconds {stopTime - startTime};
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}

#include "aoc16b.h"
#include "../aoc_16a/input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view trueAunt {R"(children: 3
cats: 7
samoyeds: 2
pomeranians: 3
akitas: 0
vizslas: 0
goldfish: 5
trees: 3
cars: 2
perfumes: 1
)"};
    constexpr std::size_t trueAuntLength {StringUtils::length(trueAunt)};
    constexpr std::array<std::string_view, trueAuntLength> trueAuntInfo {StringUtils::split<trueAuntLength>(trueAunt)};
    
    aoc16b::AuntInt aunt {aoc16b::findAunt<trueAuntLength, length>(trueAuntInfo, lines)};
    std::cout << "Aunt Sue #" << aunt << " gave the gift.\n";
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

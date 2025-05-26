#include "aoc19a.h"
#include "input.h"
#include "../../shared/StringUtils.h"
#include "../../shared/Timer.h"
#include <iostream>

int main()
{
    Timer timer {};
    
    constexpr std::string_view input {input::input};
    constexpr std::size_t length {StringUtils::length(input)};
    constexpr std::array<std::string_view, length> lines {StringUtils::split<length>(input)};
    
    constexpr std::string_view testInput {R"(H => HO
H => OH
O => HH

HOH
)"};
    constexpr std::size_t testLength {StringUtils::length(testInput)};
    constexpr std::array<std::string_view, testLength> testLines {StringUtils::split<testLength>(testInput)};
    
    aoc19a::MolInt distinct {aoc19a::countDistinctMolecules<length>(lines)};
    std::cout << "Number of distinct molecules: " << distinct << '\n';
    
    std::cout << "Time taken: " << timer.getDuration() << " s\n";
    
    return 0;
}

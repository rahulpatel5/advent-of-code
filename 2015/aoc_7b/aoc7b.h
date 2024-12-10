#ifndef AOC7B_H
#define AOC7B_H

#include "../aoc_7a/aoc7a.h"
#include "../aoc_7a/parse_string.h"
#include "../aoc_7a/bitwise.h"
#include <sstream>
#include <unordered_map>
#include <vector>

namespace aoc7b
{
    bitwise::StringIntMap parseAndResetWires(const std::string& input)
    {
        aoc7a::StringVectorMap wires1 {aoc7a::getInstructions(input)};
        bitwise::StringIntMap circuit1 {aoc7a::getCircuit(wires1)};

        // change wire b to signal for wire a
        std::string initialASignal {std::to_string(circuit1["a"])};
        aoc7a::StringVectorMap wires2 {wires1};
        wires2["b"][0] = initialASignal;
        bitwise::StringIntMap circuit2 {aoc7a::getCircuit(wires2)};
        return circuit2;
    }
}

#endif // AOC7B_H

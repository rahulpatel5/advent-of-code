#ifndef AOC24B_H
#define AOC24B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <bitset>
#include <stdexcept>
#include <functional>
#include <set>
#include <ranges>

#include <iostream>

/*
re-use solution to part 1
re-jig to output addition of x and y gates and check this gives the z gates
create function to iterate changing 4 pairs of gates and testing result
puzzle says 8 wires are changed i.e. the 4 pairs are unique
is there a smart way to approach the iterating?

do we need to re-calculate the gates?
if not, shouldn't we only care if the x, y or z gates are moved?
other gates changing shouldn't affect anything

the issue is that the swap needs to occur before anything else happens
this answers the point above, i.e. all gates can affect things

code is running too slow - not even getting past setting up combinations
need to make everything more efficient / figure out some algorithm
*/

namespace aoc24b
{
    using GateInt = unsigned long long;
    using Wire = std::string;
    using Wires = std::vector<Wire>;
    using WireValue = int;
    using WireMap = std::map<Wire, WireValue>;
    using Input = std::string;
    using Output = std::string;
    using Operator = std::string;
    // Gate consists of Input, Output and Operator
    // but too much of a faff to add all those
    // hard-coding to array length 4. Could set up more flexibly
    using Gate = std::array<Input, 4>;
    using Gates = std::vector<Gate>;
    // from looking at puzzle
    constexpr int bitSize {48};
    using Bits = std::bitset<bitSize>;
    using CheckList = std::vector<bool>;
    using GateStart = std::string;
    // swapping pairs
    using SwappedPair = std::pair<Wire, Wire>;
    using SwappedWires = std::vector<SwappedPair>;
    using UniqueWires = std::set<Wire>;
    // swap by index
    using Index = size_t;
    using SwapIndex = std::pair<Index, Index>;
    using Combination = std::vector<SwapIndex>;
    using Combinations = std::vector<Combination>;
    using UniqueIndex = std::set<Index>;

    template <std::size_t N>
    void addWiresAndGates(WireMap& wires, Gates& gates, const std::array<std::string_view, N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.find(':') != std::string_view::npos)
            {
                size_t colon {line.find(':')};
                Wire wire {line.substr(0, colon)};
                WireValue val {std::stoi(std::string(line.substr(colon + 2)))};
                wires[wire] = val;
            }
            else if (line.find("->") != std::string_view::npos)
            {
                size_t firstSpace {line.find(' ')};
                size_t secondSpace {line.find(' ', firstSpace + 1)};
                size_t equals {line.find("->")};
                Input input1 {line.substr(0, firstSpace)};
                Operator oper {line.substr(firstSpace + 1, secondSpace - (firstSpace + 1))};
                Input input2 {line.substr(secondSpace + 1, (equals - 1) - (secondSpace + 1))};
                Output output {line.substr(equals + 3)};
                gates.push_back({oper, input1, input2, output});
            }
        }
    }

    // first pass at function, before adding more complexity
    GateInt performOperation(const Gate& gate, const WireMap& wires)
    {
        if (gate.at(0) == "AND")
            return wires.at(gate.at(1)) & wires.at(gate.at(2));
        else if (gate.at(0) == "OR")
            return wires.at(gate.at(1)) | wires.at(gate.at(2));
        else if (gate.at(0) == "XOR")
            return wires.at(gate.at(1)) ^ wires.at(gate.at(2));
        else
            throw std::invalid_argument("Given non-valid operator.\n");
    }

    void operateOnGates(const Gates& gates, WireMap& wires)
    {
        // keep track of when all operations in wires has been performed
        // so we know when we're done
        int count {0};
        // this is not enough, so we track when each operation is done
        CheckList checks {};
        for (size_t i{0}; i < gates.size(); ++i)
            checks.push_back(false);
        while (count < gates.size())
        {
            for (size_t j{0}; j < gates.size(); ++j)
            {
                // skip if we did the operation already
                if (checks[j])
                    continue;
                // skip if we don't have the necessary inputs yet
                if (wires.find(gates[j].at(1)) == wires.end() || wires.find(gates[j].at(2)) == wires.end())
                    continue;
                wires[gates[j].at(3)] = performOperation(gates[j], wires);
                checks[j] = true;
                ++count;
            }
        }
    }

    GateInt getGateInt(const WireMap& wires, GateStart start)
    {
        Bits binary {};
        for (int i{0}; i < bitSize; ++i)
        {
            Wire key {start};
            if (i < 10)
            {
                key.append("0");
                key.append(std::to_string(i));
            }
            else
                key.append(std::to_string(i));
            if (wires.find(key) == wires.end())
                break;
            Bits bitmask {};
            bitmask[i] = wires.at(key);
            binary ^= bitmask;
        }
        return binary.to_ullong();
    }

    bool isSameWire(const Combination& combinations, size_t first, size_t second)
    {
        return (combinations[first].first == combinations[second].first || combinations[first].first == combinations[second].second || combinations[first].second == combinations[second].first || combinations[first].second == combinations[second].second);
    }

    bool allUniqueWires(const Combination& combinations, size_t pair1, size_t pair2, size_t pair3, size_t pair4)
    {
        // there may be a faster way to do this
        // keeping like this for now, as it's more readable
        UniqueIndex unique {};
        unique.insert(combinations.at(pair1).first);
        unique.insert(combinations.at(pair1).second);
        unique.insert(combinations.at(pair2).first);
        unique.insert(combinations.at(pair2).second);
        unique.insert(combinations.at(pair3).first);
        unique.insert(combinations.at(pair3).second);
        unique.insert(combinations.at(pair4).first);
        unique.insert(combinations.at(pair4).second);
        // hard-code that we expect 8 unique wires
        // std::cout << "uniques: " << unique.size() << '\n';
        // std::cout << "pair1: " << combinations.at(pair1).first << ' ' << combinations.at(pair1).second << ", pair2: " << combinations.at(pair2).first << ' ' << combinations.at(pair2).second << '\n';
        return unique.size() == 8;
    }

    // this is taking too long right now
    Combinations getCombinations(const Gates& gates)
    {
        // confirmed that outputs are unique
        // so we only need to collect the indexes
        Combination uniqueCombinations {};
        for (size_t i{0}; i < gates.size(); ++i)
        {
            for (size_t j{i + 1}; j < gates.size(); ++j)
                uniqueCombinations.push_back({i, j});
        }
        // std::cout << "unique combos: " << uniqueCombinations.size() << '\n';

        // now want to find combinations of 4 swapped pairs
        Combinations combinations {};
        // this is a rubbish bodge. Takes too long too
        std::cout << "unique: " << uniqueCombinations.size() << '\n';
        for (size_t a{0}; a < uniqueCombinations.size(); ++a)
        {
            std::cout << "a: " << a << '\n';
            for (size_t b{a + 1}; b < uniqueCombinations.size(); ++b)
            {
                if (isSameWire(uniqueCombinations, a, b))
                    continue;
                for (size_t c{b + 1}; c < uniqueCombinations.size(); ++c)
                {
                    if (isSameWire(uniqueCombinations, b, c))
                        continue;
                    for (size_t d{c + 1}; d < uniqueCombinations.size(); ++d)
                    {
                        if (isSameWire(uniqueCombinations, c, d))
                            continue;
                        // if (allUniqueWires(uniqueCombinations, a, b, c, d))
                        combinations.push_back({uniqueCombinations[a], uniqueCombinations[b], uniqueCombinations[c], uniqueCombinations[d]});
                    }
                }
            }
        }
        // std::cout << "final combos: " << combinations.size() << '\n';
        return combinations;
    }

    Gates swapGates(Gates gates, const Combination& combination)
    {
        for (SwapIndex pair : combination)
        {
            // getting output in the 4th (last) position
            Wire firstOutput {gates.at(pair.first).at(3)};
            gates[pair.first][3] = gates[pair.second][3];
            gates[pair.second][3] = firstOutput;
        }
        return gates;
    }

    SwappedWires getSwappedWires(const Gates& gates, const Combination& combination)
    {
        SwappedWires swapped {};
        for (const SwapIndex& pair : combination)
        {
            Wire first {gates.at(pair.first).at(3)};
            Wire second {gates.at(pair.second).at(3)};
            swapped.push_back({first, second});
        }
        return swapped;
    }

    SwappedWires swapGatesAndOperate(const Gates& gates, const WireMap& wires)
    {
        // get combinations of swapped wires
        Combinations combinations {getCombinations(gates)};
        // std::cout << "combo: " << combinations.size() << '\n';
        // how to efficiently swap pairs?
        // generating copies of Gates and WireMap will likely be slow
        std::cout << "combos: " << combinations.size() << '\n';
        for (const Combination& combination : combinations)
        {
            Gates copyGates {swapGates(gates, combination)};
            WireMap copyWires {wires};
            operateOnGates(copyGates, copyWires);
            GateInt xGates {getGateInt(copyWires, "x")};
            GateInt yGates {getGateInt(copyWires, "y")};
            GateInt zGates {getGateInt(copyWires, "z")};
            // std::cout << xGates + yGates << ' ' << zGates << '\n';
            if (xGates + yGates == zGates)
                return getSwappedWires(gates, combination);
        }
        throw std::invalid_argument("Failed to find a combination of swaps that adds up correctly.\n");
    }

    Wire sortAndCombineWires(const SwappedWires& swapped)
    {
        UniqueWires wires {};
        for (const SwappedPair& pair : swapped)
        {
            wires.insert(pair.first);
            wires.insert(pair.second);
        }
        // hard-code check that this is 8 wires, as expected
        if (wires.size() != 8)
            throw std::invalid_argument("Final collection does not have the expected 8 wires.\n");

        bool isFirstLoop {true};
        Wire final;
        for (const Wire& wire : wires)
        {
            if (!isFirstLoop)
            {
                final.append(",");
            }
            final.append(wire);
            isFirstLoop = false;
        }
        return final;
    }

    template <std::size_t N>
    Wire parseAndGetSwappedWires(const std::array<std::string_view, N>& lines)
    {
        WireMap wires {};
        Gates gates {};
        addWiresAndGates<N>(wires, gates, lines);
        SwappedWires swappedWires {swapGatesAndOperate(gates, wires)};
        return sortAndCombineWires(swappedWires);
    }
}

#endif // AOC24B_H

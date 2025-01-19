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
#include "../../shared/Timer.h"

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

can we use the fact that we have a known maximum?
i.e. the largest z gate has the most significant digit
it is z45, i.e. 2**45 = 35,184,372,088,832
could we then focus on finding combinations involving swapping that row?
not really. It still leaves an extremely large number of combinations

that feels like the solution - we know what bitwise AND should produce
if we compare the bitwise AND of the current numbers
we can then see at what points there are differences
we should then be able to cut down the number of gates we look at

this is the wrong direction. The final number is the output
it's better/easier to look at gates
e.g. x02 AND y02 -> z02
since we have x AND y = z, this must also be true on the bit level
the complexity is some gates have intermediates so we can't see x, y and z
there may be some x and y gates that are missing too
we may need to change gates that affect more than one output
looks like numbers are different by more than 8 bits (possibly by 16)

missed that we need to ADD x and y to get z
so do need to evaluate x + y = z first

too many combinations even after cutting them down
isn't the point that only a few outputs change?
so can re-assign values to swapped outputs and look at how things change?

need to cast a wider net
is this about finding inputs that contribute to z outputs
then finding inputs that don't affect outputs we want to keep the same
or, applying an even number of changes so that those outputs stay the same
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
    using Combination = std::vector<SwappedPair>;
    using Combinations = std::vector<Combination>;
    // // swap by index
    // using Index = size_t;
    // using SwapIndex = std::pair<Index, Index>;
    // using Combination = std::vector<SwapIndex>;
    // using Combinations = std::vector<Combination>;
    // gate positions that are different
    using GateBits = std::vector<int>;
    using UniqueGateBits = std::set<int>;
    // associations between gates - values 1-2: inputs, value 3: operator
    using Associations = std::map<Output, std::array<Input, 3>>;
    // associations from one input to another input, operand and output
    using GateData = std::array<Output, 3>;
    using MultiAssociations = std::map<Input, GateData>;

    template <std::size_t N>
    void addWiresAndGates(WireMap& wires, Gates& gates, Associations& associations, MultiAssociations& multiAssoc, const std::array<std::string_view, N>& lines)
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
                associations[output] = {input1, input2, oper};
                multiAssoc[input1] = {oper, input2, output};
                multiAssoc[input2] = {oper, input1, output};
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

    GateInt getGateInt(const WireMap& wires, const GateStart& start)
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

    unsigned long long getCombinationSize(size_t total, size_t choose)
    {
        unsigned long long product {1};
        for (size_t i{choose + 1}; i <= total; ++i)
        {
            product = (product * i / (i - choose));
        }
        return product;
    }

    Combinations getCombinations(const Wires& swapGates0, const Wires& swapGates1)
    {
        // confirmed that outputs are unique
        // so we only need to collect the indexes

        // first get unique pairs
        Combination uniquePairs {};
        for (const Wire& gate0 : swapGates0)
        {
            for (const Wire& gate1 : swapGates1)
            {
                if (gate0 != gate1)
                    uniquePairs.push_back({gate0, gate1});
            }
        }
        std::cout << "unique pairs: " << uniquePairs.size() << '\n';

        // want to find combinations of 4 swapped pairs
        Combinations combinations {};
        // this is a messy bodge
        for (size_t a{0}; a < uniquePairs.size(); ++a)
        {
            for (size_t b{a + 1}; b < uniquePairs.size(); ++b)
            {
                if (uniquePairs[a].first == uniquePairs[b].first || uniquePairs[a].second == uniquePairs[b].second)
                    continue;
                for (size_t c{b + 1}; c < uniquePairs.size(); ++c)
                {
                    if (uniquePairs[a].first == uniquePairs[c].first || uniquePairs[a].second == uniquePairs[c].second || uniquePairs[b].first == uniquePairs[c].first || uniquePairs[b].second == uniquePairs[c].second)
                        continue;
                    for (size_t d{c + 1}; d < uniquePairs.size(); ++d)
                    {
                        if (uniquePairs[a].first == uniquePairs[d].first || uniquePairs[a].second == uniquePairs[d].second || uniquePairs[b].first == uniquePairs[d].first || uniquePairs[b].second == uniquePairs[d].second || uniquePairs[c].first == uniquePairs[d].first || uniquePairs[c].second == uniquePairs[d].second)
                            continue;
                        
                        combinations.push_back({uniquePairs[a], uniquePairs[b], uniquePairs[c], uniquePairs[d]});
                    }
                }
            }
        }
        // for (auto combo : combinations)
        // {
        //     for (auto c : combo)
        //     {
        //         std::cout << c.first << ',' << c.second << ' ';
        //     }
        //     std::cout << '\n';
        // }
        return combinations;
    }

    Gates swapGates(Gates gates, const Combination& combination)
    {
        for (const SwappedPair& pair : combination)
        {
            // this is a wasteful way to do this
            // would be more efficient to have an index or similar
            int matches {0};
            for (Gate& gate : gates)
            {
                if (gate[3] == pair.first)
                {
                    gate[3] = pair.second;
                    ++matches;
                }
                else if (gate[3] == pair.second)
                {
                    gate[3] = pair.first;
                    ++matches;
                }
                if (matches > 1)
                    break;
            }
            // // getting output in the 4th (last) position
            // Wire firstOutput {gates.at(pair.first).at(3)};
            // gates[pair.first][3] = gates[pair.second][3];
            // gates[pair.second][3] = firstOutput;
        }
        return gates;
    }

    SwappedWires swapGatesAndOperate(const Gates& gates, const WireMap& wires, const Wires& swapGates0, const Wires& swapGates1, const MultiAssociations& multiAssoc)
    {
        // get combinations of swapped wires
        Combinations combinations {getCombinations(swapGates0, swapGates1)};
        // std::cout << "combo: " << combinations.size() << '\n';
        // how to efficiently swap pairs?
        // generating copies of Gates and WireMap will likely be slow
        std::cout << "combos: " << combinations.size() << '\n';
        int counter {0};
        Timer timer {};
        for (const Combination& combination : combinations)
        {
            if (counter > 0 && counter % 100'000 == 0)
            {
                std::cout << "loop: " << counter << '\n';
                std::cout << "time: " << timer.getDuration() << '\n';
            }
            ++counter;

            // let's change output values directly
            Gates copyGates {gates};
            WireMap copyWires {wires};
            // iterate over swapped gates
            Wires next {};
            // possible issue is not accounting for changes in multiAssoc
            for (const SwappedPair& pair : combination)
            {
                // store one output value
                WireValue stored {copyWires[pair.second]};
                copyWires[pair.second] = copyWires[pair.first];
                copyWires[pair.first] = stored;
                next.push_back(pair.first);
                next.push_back(pair.second);
            }
            while (next.size() > 0)
            {
                Wires newIter {};
                for (const Wire& wire : next)
                {
                    // as we may have a gate as input more than once
                    auto range {multiAssoc.equal_range(wire)};
                    for (auto it {range.first}; it != range.second; ++it)
                    {
                        Wire key {it->first};
                        GateData values {it->second};
                        WireValue output {};
                        if (values[0] == "AND")
                            output = copyWires[key] & copyWires[values[1]];
                        else if (values[0] == "OR")
                            output = copyWires[key] | copyWires[values[1]];
                        else if (values[0] == "XOR")
                            output = copyWires[key] ^ copyWires[values[1]];
                        copyWires[values[2]] = output;
                        newIter.push_back(values[2]);
                    }
                }
                next = newIter;
            }
            GateInt xGates {getGateInt(copyWires, "x")};
            GateInt yGates {getGateInt(copyWires, "y")};
            GateInt zGates {getGateInt(copyWires, "z")};
            if (xGates + yGates == zGates)
                return combination;

            // Gates copyGates {swapGates(gates, combination)};
            // WireMap copyWires {wires};
            // operateOnGates(copyGates, copyWires);
            // GateInt xGates {getGateInt(copyWires, "x")};
            // GateInt yGates {getGateInt(copyWires, "y")};
            // GateInt zGates {getGateInt(copyWires, "z")};
            // // std::cout << xGates + yGates << ' ' << zGates << '\n';
            // if (xGates + yGates == zGates)
            //     return combination;
        }
        throw std::invalid_argument("Failed to find a combination of swaps that adds up correctly.\n");
    }

    Wires getOptionsForSwapping(const GateBits& diff, const Associations& associations, const WireMap& wires, const Wire& start)
    {
        Wires swaps {};
        for (int gate : diff)
        {
            // get z gate
            Wire key {start};
            if (gate < 10)
            {
                key.append("0");
                key.append(std::to_string(gate));
            }
            else
                key.append(std::to_string(gate));
            
            // now find associated gates
            // go one layer deep for now

            // if we have XOR, either input could change
            // std::cout << "a " << key << ' ' << associations.at(key).size() << "\n\t";
            // for (auto a : associations.at(key))
            // {
            //     std::cout << a << ' ';
            // }
            // std::cout << '\n';
            if (associations.at(key)[2] == "XOR")
            {
                // std::cout << "b\n";
                swaps.push_back(associations.at(key)[0]);
                swaps.push_back(associations.at(key)[1]);
            }
            // if we have AND or OR, change inputs that don't match target
            else
            {
                // we've set up diff to collect either where z = 0 or 1
                // std::cout << "c\n";
                WireValue target {wires.at(key)};
                if (wires.at(associations.at(key)[0]) != target)
                    swaps.push_back(associations.at(key)[0]);
                if (wires.at(associations.at(key)[1]) != target)
                    swaps.push_back(associations.at(key)[1]);
            }
        }
        return swaps;
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
        Associations associations {};
        MultiAssociations multiAssoc {};
        addWiresAndGates<N>(wires, gates, associations, multiAssoc, lines);

        // generate output based on unchanged, original setup
        WireMap origWires {wires};
        Gates origGates {gates};
        operateOnGates(origGates, origWires);
        GateInt origXGates {getGateInt(origWires, "x")};
        GateInt origYGates {getGateInt(origWires, "y")};
        GateInt origZGates {getGateInt(origWires, "z")};

        // compare the sum of x and y vs z
        Bits addXY {origXGates + origYGates};
        Bits bitsOrigZGates {origZGates};
        Bits xorXYZ {addXY ^ bitsOrigZGates};
        std::cout << "X:\t\t" << std::bitset<48>(origXGates) << "\nY:\t\t" << std::bitset<48>(origYGates) << "\nX + Y:\t\t" << addXY << "\nZ:\t\t" << bitsOrigZGates << "\nXY xor Z:\t" << xorXYZ << '\n';
        // use this to find which bit positions (i.e. gates) differ
        // divide these into which have a 0 or 1 for z gates
        // i.e. 0 where it x + y = 0 but z = 1, or vice-versa
        GateBits diffBits0 {};
        GateBits diffBits1 {};
        Bits bitsOrigXGates {origXGates};
        Bits bitsOrigYGates {origYGates};
        for (size_t i{0}; i < bitSize; ++i)
        {
            if (i == 0)
            {
                if (xorXYZ.test(i))
                {
                    if (bitsOrigZGates.test(i))
                        diffBits0.push_back(i);
                    else
                        diffBits1.push_back(i);
                }
            }
            else
            {
                // account for later differences being due to carry over
                if (xorXYZ.test(i))
                {
                    // if no carry over
                    if ((bitsOrigXGates[i - 1] == 0 && bitsOrigYGates[i - 1] == 0 && addXY[i - 1] == 0) || (bitsOrigXGates[i - 1] ^ bitsOrigYGates[i - 1] == 1 && addXY[i - 1] == 1))
                    {
                        if (bitsOrigZGates.test(i))
                            diffBits0.push_back(i);
                        else
                            diffBits1.push_back(i);
                    }
                    // if there is carry over
                    else
                    {
                        // if xorXYZ was off for i - 1, no point repeating
                        if (xorXYZ.test(i))
                            continue;
                        // keep going back until we get to carry over start
                        size_t back {1};
                        // to avoid overflow
                        while (i - (back - 1) > 1)
                        {
                            // if no more carry over, end
                            if ((bitsOrigXGates[i - back] == 0 && bitsOrigYGates[i - back] == 0 && addXY[i - back] == 0) || (bitsOrigXGates[i - back] ^ bitsOrigXGates[i - back] == 1 && addXY[i - back] == 1))
                            {
                                // swap logic above due to carry over
                                if (bitsOrigZGates.test(i - back))
                                    diffBits1.push_back(i - back);
                                else
                                    diffBits0.push_back(i);
                            }

                            ++back;
                        }
                    }
                }
            }
        }

        // let's remove duplicates
        // bit wasteful but this only happens once
        UniqueGateBits uniqueDiff0 {diffBits0.begin(), diffBits0.end()};
        UniqueGateBits uniqueDiff1 {diffBits1.begin(), diffBits1.end()};
        GateBits differentBits0 {uniqueDiff0.begin(), uniqueDiff0.end()};
        GateBits differentBits1 {uniqueDiff1.begin(), uniqueDiff1.end()};

        // now we find possible gates to swap
        Wires gateSwapOptions0 {getOptionsForSwapping(differentBits0, associations, origWires, "z")};
        Wires gateSwapOptions1 {getOptionsForSwapping(differentBits1, associations, origWires, "z")};

        for (const Wire& swap : gateSwapOptions0)
            std::cout << swap << ' ';
        std::cout << '\n';
        std::cout << '\n';
        for (const Wire& swap : gateSwapOptions1)
            std::cout << swap << ' ';
        std::cout << '\n';

        std::cout << "2\n";
        SwappedWires swappedWires {swapGatesAndOperate(gates, wires, gateSwapOptions0, gateSwapOptions1, multiAssoc)};
        return sortAndCombineWires(swappedWires);
        // return "";
    }
}

#endif // AOC24B_H

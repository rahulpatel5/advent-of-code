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

looking at connections from outputs to all previous inputs
it looks like some are affected by many/most wires
we can try just looking at wires that affect one output

still generating too many combinations
let's try and separate z gates from xy gates
this should reduce the number of combinations
could split xy gates so that ones contributing to same x/y don't join
but not sure how to do this, so leave as something to try if needed

TO FIX:
    swapGatesAndOperate() not working right (while loop)
    wires being changed doesn't include x, y or z wires?
    check if diffBits is collecting right numbers (missing 14-17, 29-30?)
    re-consider logic for how diffBits was set up (why only inputs?)
    fix number of combinations getting too large

one issue is having to re-run operateOnGates() each time
but if we run the function once and collect the state of gates at each pt
we may be able to short-circuit and jump to earliest switch that was made?
may need to add logic to skip other lines not affected?
or can we use origGates itself?

an easier change may be to not allow pairs with the same output value
no point swapping if we end up with the same value (waste of time)
this would reduce the number of combinations and save time
issue is if these are wires with several inputs and are affected by those
does it make sense to only look at wires with two inputs?
e.g. as a test to see if it helps the script to run to the end

IDEAS TO TRY:
    change wires and gates to using int, with a map from str to int and v-v
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

    using GateBits = std::vector<int>;
    using UniqueGateBits = std::set<int>;
    // associations between gates - values 1-2: inputs, value 3: operator
    using Associations = std::map<Output, std::array<Input, 3>>;
    // associations from one input to another input, operand and output
    using GateData = std::array<Output, 3>;
    // associations from each input to rest of gate data
    using MultiAssociations = std::map<Input, GateData>;
    using UniqueInputs = std::set<Input>;
    using OutputToInputs = std::map<Output, UniqueInputs>;

    using WireIndex = std::int16_t;
    using WireIndexes = std::vector<WireIndex>;
    using UniqueWireIndex = std::set<WireIndex>;
    using WireToIndex = std::map<Wire, WireIndex>;
    using IndexToWire = std::map<WireIndex, Wire>;

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

    OutputToInputs mapInputsToOutput(const Gates& gates, const Associations& associations)
    {
        OutputToInputs map {};
        for (size_t i{0}; i < gates.size(); ++i)
        {
            Output key {gates.at(i).at(3)};
            // std::cout << "0 " << key << '\n';
            if (associations.find(key) == associations.end())
                continue;
            
            // std::cout << "1\n";
            map[key].insert(associations.at(key).at(0));
            map[key].insert(associations.at(key).at(1));
            UniqueInputs current {};
            current.insert(associations.at(key).at(0));
            current.insert(associations.at(key).at(1));
            while (current.size() > 0)
            {
                UniqueInputs next {};
                for (const Input& input : current)
                {
                    // if input is output too, we'll collect those inputs
                    if (associations.find(input) == associations.end())
                        continue;
                    for (const Input& nextInput : associations.at(input))
                    {
                        if (nextInput == "OR" || nextInput == "AND" || nextInput == "XOR")
                            continue;
                        map[key].insert(nextInput);
                        next.insert(nextInput);
                    }
                }
                current = next;
            }
        }
        return map;
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

    Combinations getCombinations(const Wires& swapGates, const WireMap& wires, const WireMap& origWires)
    {
        // confirmed that outputs are unique
        // so we only need to collect the indexes

        // first get unique pairs
        Combination uniquePairs {};
        for (const Wire& gate0 : swapGates)
        {
            for (const Wire& gate1 : swapGates)
            {
                // we're just using if the final value of origWires match
                // this doesn't account for values changing by other inputs
                if (gate0 != gate1 && origWires.at(gate0) != origWires.at(gate1))
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

    SwappedWires swapGatesAndOperate(const Gates& gates, const WireMap& wires, const Wires& swapZGates, const Wires& swapXYGates, const MultiAssociations& multiAssoc, const WireMap& origWires)
    {
        // get combinations of swapped wires
        // for z gates
        // Combinations combinationsZ {getCombinations(swapZGates, wires, origWires)};
        // for x and y gates
        Combinations combinationsXY {getCombinations(swapXYGates, wires, origWires)};
        // combine the ... combinations
        Combinations combinations {};
        // combinations.insert(combinations.end(), combinationsZ.begin(), combinationsZ.end());
        combinations.insert(combinations.end(), combinationsXY.begin(), combinationsXY.end());

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
            // WireMap copyWires {wires};
            // iterate over swapped gates
            // UniqueWires next {};
            // possible issue is not accounting for changes in multiAssoc
            // for (const SwappedPair& pair : combination)
            // {
            //     // store one output value
            //     WireValue stored {copyWires[pair.second]};
            //     auto secondWire {copyWires.extract(pair.second)};
            //     secondWire.key() = copyWires[pair.first];
            //     auto firstWire {copyWires.extract(pair.first)};
            //     firstWire.key() = stored;
            //     next.insert(pair.first);
            //     next.insert(pair.second);
            // }

            // CHANGE HERE
            // this isn't working right
            // does the order matter?
            // i.e. do we need to go in same order as operateOnGates()?
            // while (next.size() > 0)
            // {
            //     UniqueWires newIter {};
            //     for (const Wire& wire : next)
            //     {
            //         // as we may have a gate as input more than once
            //         auto range {multiAssoc.equal_range(wire)};
            //         for (auto it {range.first}; it != range.second; ++it)
            //         {
            //             Wire key {it->first};
            //             GateData values {it->second};
            //             WireValue output {};
            //             if (values[0] == "AND")
            //                 output = copyWires[key] & copyWires[values[1]];
            //             else if (values[0] == "OR")
            //                 output = copyWires[key] | copyWires[values[1]];
            //             else if (values[0] == "XOR")
            //                 output = copyWires[key] ^ copyWires[values[1]];
            //             copyWires[values[2]] = output;
            //             newIter.insert(values[2]);
            //         }
            //     }
            //     next = newIter;
            // }

            Gates copyGates {swapGates(gates, combination)};
            WireMap copyWires {wires};
            operateOnGates(copyGates, copyWires);
            
            GateInt xGates {getGateInt(copyWires, "x")};
            GateInt yGates {getGateInt(copyWires, "y")};
            GateInt zGates {getGateInt(copyWires, "z")};
            std::cout << xGates + yGates << ' ' << zGates << '\n';
            if (xGates + yGates == zGates)
                return combination;
        }
        throw std::invalid_argument("Failed to find a combination of swaps that adds up correctly.\n");
    }

    Wires getOptionsForSwapping(const GateBits& diff, const Associations& associations, const WireMap& wires, const Wire& z)
    {
        Wires swaps {};
        for (int gate : diff)
        {
            // get z gate
            Wire zKey {z};
            if (gate < 10)
            {
                zKey.append("0");
                zKey.append(std::to_string(gate));
            }
            else
                zKey.append(std::to_string(gate));
            
            swaps.push_back(zKey);

            continue;
            
            // now find associated gates
            // go one layer deep for now

            // if we have XOR, either input could change
            // std::cout << "a " << zKey << ' ' << associations.at(zKey).size() << "\n\t";
            // for (auto a : associations.at(zKey))
            // {
            //     std::cout << a << ' ';
            // }
            // std::cout << '\n';
            if (associations.at(zKey)[2] == "XOR")
            {
                // std::cout << "b\n";
                swaps.push_back(associations.at(zKey)[0]);
                swaps.push_back(associations.at(zKey)[1]);
            }
            // if we have AND or OR, change inputs that don't match target
            else
            {
                // we've set up diff to collect either where z = 0 or 1
                // note this may miss wires that change by other inputs
                // std::cout << "c\n";
                WireValue target {wires.at(zKey)};
                if (wires.at(associations.at(zKey)[0]) != target)
                    swaps.push_back(associations.at(zKey)[0]);
                if (wires.at(associations.at(zKey)[1]) != target)
                    swaps.push_back(associations.at(zKey)[1]);
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
        // note: multiAssoc won't contain z gates since not present yet
        addWiresAndGates<N>(wires, gates, associations, multiAssoc, lines);

        // UniqueWires unique {};
        // for (const auto& [key, value] : wires)
        //     unique.insert(key);
        // for (const Gate& g : gates)
        // {
        //     unique.insert(g.at(1));
        //     unique.insert(g.at(2));
        //     unique.insert(g.at(3));
        // }
        // std::cout << "unique wires: " << unique.size() << '\n';

        // let's get all layers of inputs for each output
        OutputToInputs inputsForOutput {mapInputsToOutput(gates, associations)};
        // std::cout << "input output size: " << inputsForOutput.size() << '\n';
        // for (auto [key, value] : inputsForOutput)
        // {
        //     if (value.size() > 2)
        //         continue;
        //     std::cout << key << ": ";
        //     for (const Input& input : value)
        //         std::cout << input << ' ';
        //     std::cout << '\n';
        // }

        // generate output based on unchanged, original setup
        WireMap origWires {wires};
        Gates origGates {gates};
        operateOnGates(origGates, origWires);
        GateInt origXGates {getGateInt(origWires, "x")};
        GateInt origYGates {getGateInt(origWires, "y")};
        GateInt origZGates {getGateInt(origWires, "z")};
        std::cout << "orig: " << (origXGates + origYGates) << " = " << origZGates << '\n';

        // compare the sum of x and y vs z
        Bits addXY {origXGates + origYGates};
        Bits bitsOrigZGates {origZGates};
        Bits xorXYZ {addXY ^ bitsOrigZGates};
        // std::cout << "xorXYZ: " << xorXYZ << '\n';
        // std::cout << "X:\t\t" << std::bitset<48>(origXGates) << "\nY:\t\t" << std::bitset<48>(origYGates) << "\nX + Y:\t\t" << addXY << "\nZ:\t\t" << bitsOrigZGates << "\nXY xor Z:\t" << xorXYZ << '\n';
        // use this to find which bit positions (i.e. gates) differ
        GateBits diffBits {};
        Bits bitsOrigXGates {origXGates};
        Bits bitsOrigYGates {origYGates};
        for (size_t i{0}; i < bitSize; ++i)
        {
            if (xorXYZ.test(i))
                diffBits.push_back(i);
        }

        // let's remove duplicates (bit wasteful)
        UniqueGateBits uniqueDiff {diffBits.begin(), diffBits.end()};
        GateBits differentBits {uniqueDiff.begin(), uniqueDiff.end()};

        // now we find possible wires to swap connected to z gates
        Wires gateZSwapOptions {getOptionsForSwapping(differentBits, associations, origWires, "z")};

        // let's find possible wires connected to x or y gates
        UniqueWires gateXYSwaps {};
        for (const auto& [key, values] : inputsForOutput)
        {
            // we're skipping if there more than 2 associated inputs
            if (values.size() > 2)
                continue;
            
            for (int b : differentBits)
            {
                // bit wasteful to set these strings (Wire) up each time
                Wire postFix;
                if (b < 10)
                    postFix.append("0");
                postFix.append(std::to_string(b));
                
                Wire xKey {"x"};
                xKey.append(postFix);
                Wire yKey {"y"};
                yKey.append(postFix);

                gateXYSwaps.insert(xKey);
                gateXYSwaps.insert(yKey);

                continue;

                if (values.find(xKey) != values.end())
                    gateXYSwaps.insert(key);
                if (values.find(yKey) != values.end())
                    gateXYSwaps.insert(key);
            }
        }
        Wires gateXYSwapOptions {gateXYSwaps.begin(), gateXYSwaps.end()};

        // for (const Wire& swap : gateZSwapOptions)
        // {
        //     std::cout << swap << ": ";
        //     if (inputsForOutput.find(swap) != inputsForOutput.end())
        //         std::cout << inputsForOutput.at(swap).size() << '\n';
        //     else
        //         std::cout << "0\n";
        // }
        // std::cout << '\n';
        // for (const Wire& swap : gateXYSwapOptions)
        // {
        //     std::cout << swap << ": ";
        //     if (inputsForOutput.find(swap) != inputsForOutput.end())
        //         std::cout << inputsForOutput.at(swap).size() << '\n';
        //     else
        //         std::cout << "0\n";
        // }
        std::cout << "z swap:  ";
        for (const Wire& swap : gateZSwapOptions)
            std::cout << swap << ' ';
        std::cout << '\n';
        std::cout << "xy swap: ";
        for (const Wire& swap : gateXYSwapOptions)
            std::cout << swap << ' ';
        std::cout << '\n';

        // std::cout << "2\n";
        SwappedWires swappedWires {swapGatesAndOperate(gates, wires, gateZSwapOptions, gateXYSwapOptions, multiAssoc, origWires)};
        return sortAndCombineWires(swappedWires);
        // return "";
    }
}

#endif // AOC24B_H

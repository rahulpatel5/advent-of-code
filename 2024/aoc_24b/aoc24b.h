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
#include "WireGraph.h"

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
    change wires to using int, with a map from str to int and vice-versa
this seems to have sped up the code (3x?)
think the rest requires being smarter about choosing options
can we use gate info to identify what needs to change?
e.g. if we have AND and 1 value is 0, we know that needs to change
or if both values are 0, then both need to change

check if it would speed up to collect x, y, z keys in a map
could slow down to generate strings every time

optimising is not achieving much
IDEA TO TRY:
    set up a unidirectional graph with wires as vertices
    have edges point from inputs to outputs
    means multiple edges per vertex
    could then work backwards from outputs that are off
    and see which wires (vertices could change)
    could use weighting (int) to show if 1 or 0
    or might need to use that for AND/OR/XOR?

graph seems faster
IDEAS TO TRY:
    for checkOutputDifferences() check if relevant difference is present
    otherwise break
    try identifying limited number of inputs?

can try iterating through pairs first and find those that make most diff
could start with the one making the biggest diff and doesn't go over
another option is to group by how much difference the pair makes
e.g. the ones that make the most, ones making the least, and some between
note that some differences might change depending on other swaps

did I forget to account for permutations when swapping wires?

sort diff vector and swaps at same time, using map
we want to order with swaps getting closest to the target
so we combine swaps that may have the best chance of solving the problem
we'll also want to separate inputs from the same wire
otherwise we're swapping wires that contribute to the same output
try separating, though risk of missing the solution this way

a for loop of combinations is too much computational time
an alternative is a while loop
1. start with a selection of 4 swaps (make sure none are same)
2. take difference of x + y and z, and store this
3. change one of the swaps, get the difference and store
4. repeat this n more times for the same position (e.g. 10 times total)
5. find the swap in that position that was closest
6. now choose another of the swaps, i.e. another position, and repeat 2-5
7. keep going until get to x + y == z
questions include:
- what combinations to iterate over
- whether to have separate combination lists for each position
- how to make sure we're not just iterating over the same positions
could use the layers, i.e. inputs for different positions
i.e. have 4 layers and use those to swap positions
that saves other manipulations to construct swaps
*/

namespace aoc24b
{
    using GateInt = unsigned long long;
    using WireStr = std::string;
    using Wire = std::int16_t;

    using StrToIndex = std::map<WireStr, Wire>;
    using IndexToStr = std::map<Wire, WireStr>;

    using Wires = std::vector<Wire>;
    using UniqueWires = std::set<Wire>;
    using WireValue = std::int16_t;
    using WireMap = std::map<Wire, WireValue>;

    using Input = Wire;
    using Output = Wire;
    using Operator = Wire;
    using Gate = std::array<Wire, 4>;
    using Gates = std::vector<Gate>;

    // from looking at puzzle
    constexpr int bitSize {48};
    using Bits = std::bitset<bitSize>;
    using CheckList = std::vector<bool>;
    using GateStart = Wire;
    // swapping pairs
    using SwappedPair = std::pair<Wire, Wire>;
    using SwappedWires = std::vector<SwappedPair>;
    using Combination = std::vector<SwappedPair>;
    using Combinations = std::vector<Combination>;

    using GateBits = std::vector<std::int16_t>;
    using UniqueGateBits = std::set<std::int16_t>;
    // associations between gates - values 1-2: inputs, value 3: operator
    using Associations = std::map<Output, std::array<Wire, 3>>;

    using Power = unsigned long long;
    using Powers = std::array<Power, bitSize>;

    using GateInts = std::vector<GateInt>;
    using UniqueGateInt = std::set<GateInt>;
    using DiffInt = long long;
    using DiffMap = std::map<DiffInt, SwappedWires>;
    using DiffWires = std::map<Wire, Wires>;
    using WireLayers = std::vector<Wires>;
    using WireConversion = std::map<Wire, Wire>;

    // set values for AND, OR, and XOR
    Wire operAND {-1};
    Wire operOR {-2};
    Wire operXOR {-3};

    template <std::size_t N>
    Wire addWiresAndGates(WireMap& wires, Gates& gates, Associations& associations, const std::array<std::string_view, N>& lines, StrToIndex& strToIndexMap, IndexToStr& indexToStrMap)
    {
        Wire wireCount {0};
        for (std::string_view line : lines)
        {
            if (line.find(':') != std::string_view::npos)
            {
                size_t colon {line.find(':')};
                WireStr wire {line.substr(0, colon)};
                WireValue val {static_cast<Wire>(std::stoi(std::string(line.substr(colon + 2))))};
                if (strToIndexMap.find(wire) != strToIndexMap.end())
                    wires[strToIndexMap.at(wire)] = val;
                else
                {
                    strToIndexMap[wire] = wireCount;
                    indexToStrMap[wireCount] = wire;
                    wires[wireCount] = val;
                    ++wireCount;
                }
            }
            else if (line.find("->") != std::string_view::npos)
            {
                size_t firstSpace {line.find(' ')};
                size_t secondSpace {line.find(' ', firstSpace + 1)};
                size_t equals {line.find("->")};

                WireStr input1Str {line.substr(0, firstSpace)};
                Input input1;
                if (strToIndexMap.find(input1Str) != strToIndexMap.end())
                    input1 = strToIndexMap.at(input1Str);
                else
                {
                    strToIndexMap[input1Str] = wireCount;
                    indexToStrMap[wireCount] = input1Str;
                    input1 = wireCount;
                    ++wireCount;
                }
                WireStr operStr {line.substr(firstSpace + 1, secondSpace - (firstSpace + 1))};
                Operator oper;
                if (strToIndexMap.find(operStr) != strToIndexMap.end())
                    oper = strToIndexMap.at(operStr);
                else
                {
                    strToIndexMap[operStr] = wireCount;
                    indexToStrMap[wireCount] = operStr;
                    oper = wireCount;
                    ++wireCount;
                }

                WireStr input2Str {line.substr(secondSpace + 1, (equals - 1) - (secondSpace + 1))};
                Input input2;
                if (strToIndexMap.find(input2Str) != strToIndexMap.end())
                    input2 = strToIndexMap.at(input2Str);
                else
                {
                    strToIndexMap[input2Str] = wireCount;
                    indexToStrMap[wireCount] = input2Str;
                    input2 = wireCount;
                    ++wireCount;
                }

                WireStr outputStr {line.substr(equals + 3)};
                Output output;
                if (strToIndexMap.find(outputStr) != strToIndexMap.end())
                    output = strToIndexMap.at(outputStr);
                else
                {
                    strToIndexMap[outputStr] = wireCount;
                    indexToStrMap[wireCount] = outputStr;
                    output = wireCount;
                    ++wireCount;
                }

                gates.push_back({oper, input1, input2, output});
                associations[output] = {input1, input2, oper};
            }
        }
        return wireCount;
    }

    void collectXYZWires(Wires& xWires, Wires& yWires, Wires& zWires, const StrToIndex& strToIndexMap)
    {
        for (int i{0}; i < bitSize; ++i)
        {
            WireStr xKey {"x"};
            WireStr yKey {"y"};
            WireStr zKey {"z"};
            WireStr postFix;
            if (i < 10)
            {
                postFix.append("0");
            }
            postFix.append(std::to_string(i));
            xKey.append(postFix);
            yKey.append(postFix);
            zKey.append(postFix);

            if (strToIndexMap.find(xKey) != strToIndexMap.end())
                xWires.push_back(strToIndexMap.at(xKey));
            if (strToIndexMap.find(yKey) != strToIndexMap.end())
                yWires.push_back(strToIndexMap.at(yKey));
            if (strToIndexMap.find(zKey) != strToIndexMap.end())
                zWires.push_back(strToIndexMap.at(zKey));
        }
    }

    GateInt performOperation(const Gate& gate, const WireMap& wires)
    {
        if (gate.at(0) == operAND)
            return wires.at(gate.at(1)) & wires.at(gate.at(2));
        else if (gate.at(0) == operOR)
            return wires.at(gate.at(1)) | wires.at(gate.at(2));
        else if (gate.at(0) == operXOR)
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

    constexpr Powers getPowersOfTwo()
    {
        Powers powers {};
        Power pow {1};
        for (int i{0}; i < bitSize; ++i)
        {
            powers[i] = pow;
            pow *= 2;
        }
        return powers;
    }

    void graphSetup(WireGraph& graph, const Associations& associations, const WireMap& wires)
    {
        for (const auto& [output, otherWires] : associations)
        {
            graph.addEdge(output, otherWires.at(0));
            graph.addEdge(output, otherWires.at(1));
            // set operation to 1 = AND, 2 = OR, 3 = XOR
            graph.setOper(output, -otherWires.at(2));
            if (wires.find(output) != wires.end())
                graph.setValue(output, wires.at(output));
            // some inputs aren't traversed
            // so let's cover them for completeness
            // this might set them multiple times (inefficient)
            if (wires.find(otherWires.at(0)) != wires.end())
                graph.setValue(otherWires.at(0), wires.at(otherWires.at(0)));
            if (wires.find(otherWires.at(1)) != wires.end())
                graph.setValue(otherWires.at(1), wires.at(otherWires.at(1)));
        }
    }

    GateInt getGraphInt(WireGraph& graph, const Wires& keyWires, const Powers& powersOfTwo)
    {
        GateInt gateInt {0};
        for (size_t i{0}; i < keyWires.size(); ++i)
        {
            gateInt += graph.getValue(keyWires.at(i)) * powersOfTwo.at(i);
        }
        return gateInt;
    }

    Wires getXYZWires(const GateBits& diffBits, const StrToIndex& strToIndexMap, WireConversion& wireConv)
    {
        Wires xyz {};
        for (const Wire& w : diffBits)
        {
            WireStr postFix;
            if (w < 10)
                postFix.append("0");
            postFix.append(std::to_string(w));

            // WireStr xKey {"x"};
            // WireStr yKey {"y"};
            WireStr zKey {"z"};
            // xKey.append(postFix);
            // yKey.append(postFix);
            zKey.append(postFix);

            // if (strToIndexMap.find(xKey) != strToIndexMap.end())
                // xyz.push_back(strToIndexMap.at(xKey));
            // if (strToIndexMap.find(yKey) != strToIndexMap.end())
                // xyz.push_back(strToIndexMap.at(yKey));
            if (strToIndexMap.find(zKey) != strToIndexMap.end())
            {
                xyz.push_back(strToIndexMap.at(zKey));
                wireConv[w] = strToIndexMap.at(zKey);
            }
        }
        return xyz;
    }

    // void testWireSwaps(const WireGraph& graph, SwappedWires& swaps, GateInts& differences, const Wires& xWires, const Wires& yWires, const Wires& zWires, const Powers& powersOfTwo, const Wires& candidates, const GateInt& origXYGraph, const GateInt& origZGraph)
    // {
    //     for (size_t i{0}; i < candidates.size(); ++i)
    //     {
    //         for (size_t j{i + 1}; j < candidates.size(); ++j)
    //         {
    //             WireGraph copyGraph {graph};
    //             copyGraph.swapNodes(candidates.at(i), candidates.at(j));
    //             copyGraph.updateOutputs(candidates.at(i));
    //             copyGraph.updateOutputs(candidates.at(j));
    //             GateInt swapXGraph {getGraphInt(copyGraph, xWires, powersOfTwo)};
    //             GateInt swapYGraph {getGraphInt(copyGraph, yWires, powersOfTwo)};
    //             GateInt swapZGraph {getGraphInt(copyGraph, zWires, powersOfTwo)};
    //             GateInt swapXYGraph {swapXGraph + swapYGraph};

    //             DiffInt diff {static_cast<DiffInt>(swapXYGraph - swapZGraph)};
    //             if (diff < 0)
    //                 diff = -diff;
    //             differences.push_back(diff);

    //             swaps.push_back({candidates.at(i), candidates.at(j)});
    //         }
    //     }
    // }

    void swapWires(SwappedWires& swaps, const WireLayers& layers)
    {
        for (size_t a{0}; a < layers.size(); ++a)
        {
            for (size_t i{0}; i < layers.at(a).size(); ++i)
            {
                for (size_t b{a + 1}; b < layers.size(); ++b)
                {
                    for (size_t j{0}; j < layers.at(b).size(); ++j)
                    {
                        swaps.push_back({layers.at(a).at(i), layers.at(b).at(j)});
                    }
                }
            }
        }
    }

    WireStr sortAndCombineWires(const SwappedWires& swapped, const IndexToStr& indexToStrMap)
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
        WireStr final;
        for (const Wire& wire : wires)
        {
            if (!isFirstLoop)
            {
                final.append(",");
            }
            final.append(indexToStrMap.at(wire));
            isFirstLoop = false;
        }
        return final;
    }

    template <std::size_t N>
    WireStr parseAndGetSwappedWires(const std::array<std::string_view, N>& lines)
    {
        // store wires as int (int_16t), to save space/time
        // we'll need maps to convert back and forth
        StrToIndex strToIndexMap {};
        IndexToStr indexToStrMap {};
        // let's store AND, OR, and XOR as negative indexes
        // this avoids them getting in the way of the wires
        strToIndexMap["AND"] = operAND;
        strToIndexMap["OR"] = operOR;
        strToIndexMap["XOR"] = operXOR;
        indexToStrMap[operAND] = "AND";
        indexToStrMap[operOR] = "OR";
        indexToStrMap[operXOR] = "XOR";

        WireMap wires {};
        Gates gates {};
        Associations associations {};
        // main effect of function is to populate above variables
        // we collect the total number of unique wires as a side effect
        Wire total {addWiresAndGates<N>(wires, gates, associations, lines, strToIndexMap, indexToStrMap)};

        std::cout << "total: " << total << '\n';

        // generate output based on original, unchanged setup
        WireMap origWires {wires};
        Gates origGates {gates};
        operateOnGates(origGates, origWires);

        // we can save time by collecting x, y and z wires
        Wires xWires {};
        Wires yWires {};
        Wires zWires {};
        collectXYZWires(xWires, yWires, zWires, strToIndexMap);
        // also collect powers of 2
        constexpr Powers powersOfTwo {getPowersOfTwo()};

        // create the graph class object
        WireGraph graph {total};
        graphSetup(graph, associations, origWires);
        GateInt xGraph {getGraphInt(graph, xWires, powersOfTwo)};
        GateInt yGraph {getGraphInt(graph, yWires, powersOfTwo)};
        GateInt zGraph {getGraphInt(graph, zWires, powersOfTwo)};
        GateInt xyGraph {xGraph + yGraph};

        // compare the sum of x and y vs z
        Bits addXY {xGraph + yGraph};
        Bits bitsOrigZGates {zGraph};
        Bits xorXYZ {addXY ^ bitsOrigZGates};
        // use this to find which bit positions (i.e. gates) differ
        GateBits diffBits {};
        for (size_t i{0}; i < bitSize; ++i)
        {
            if (xorXYZ.test(i))
            {
                diffBits.push_back(i);
                // std::cout << i << '\n';
            }
        }

        // get z wires associated with these different bits
        WireConversion wireConv {};
        Wires possibleSwaps {getXYZWires(diffBits, strToIndexMap, wireConv)};

        std::cout << "number of possible swaps " << possibleSwaps.size() << '\n';

        // collect wires that act as inputs to these, i.e. candidate swaps
        UniqueWires uniqueCandidates {};
        DiffWires diffWires {};
        Wires temp {};
        for (const Wire& swap : possibleSwaps)
        {
            uniqueCandidates.insert(swap);
            temp = graph.getAllInputs(swap);
            uniqueCandidates.insert(temp.begin(), temp.end());
            diffWires[swap] = temp;
            // std::cout << swap << ' ' << temp.size() << '\n';
        }
        std::cout << "number of candidates: " << uniqueCandidates.size() << '\n';
        Wires candidates {uniqueCandidates.begin(), uniqueCandidates.end()};

        WireLayers tempLayers {};
        temp = {};
        Wire previous {};
        bool firstLoop {true};
        for (const Wire& w : diffBits)
        {
            if (firstLoop)
            {
                temp.push_back(w);
                previous = w;
                firstLoop = false;
            }
            else if (w == previous + 1)
            {
                temp.push_back(w);
                previous = w;
            }
            else
            {
                tempLayers.push_back(temp);
                temp = {w};
                previous = w;
            }
        }
        tempLayers.push_back(temp);

        // for (auto fl : tempLayers)
        // {
        //     for (auto sl : fl)
        //     {
        //         std::cout << sl << ' ';
        //     }
        //     std::cout << '\n';
        // }

        // for (auto [key, values] : diffWires)
        // {
        //     std::cout << key << ": ";
        //     for (auto v : values)
        //         std::cout << v << ' ';
        //     std::cout << '\n';
        // }

        WireLayers layers {};

        for (const Wires& l : tempLayers)
        {
            Wire index {l.front()};
            Wire halfway {static_cast<Wire>((l.front() + l.back()) / 2)};
            Wires firstHalfLayer {};
            Wires secondHalfLayer {};
            while (index <= l.back())
            {
                if (index <= halfway)
                    firstHalfLayer.insert(firstHalfLayer.end(), diffWires.at(wireConv.at(index)).begin(), diffWires.at(wireConv.at(index)).end());
                else
                    secondHalfLayer.insert(secondHalfLayer.end(), diffWires.at(wireConv.at(index)).begin(), diffWires.at(wireConv.at(index)).end());
                ++index;
            }
            layers.push_back(firstHalfLayer);
            layers.push_back(secondHalfLayer);
        }
        std::cout << "(onion) layers: " << layers.size() << '\n';

        // for (auto l : layers)
        // {
        //     for (auto v : l)
        //         std::cout << v << ' ';
        //     std::cout << '\n';
        // }

        SwappedWires swaps {};
        swapWires(swaps, layers);
        std::cout << "swaps: " << swaps.size() << '\n';

        // for (auto s : swaps)
        //     std::cout << s.first << ' ' << s.second << '\n';

        // for (auto fl : layers)
        // {
        //     for (auto sl : fl)
        //         std::cout << sl << ' ';
        //     std::cout << '\n';
        // }

        // SwappedWires swaps {};
        // GateInts diffs {};
        // testWireSwaps(graph, swaps, diffs, xWires, yWires, zWires, powersOfTwo, candidates, xyGraph, zGraph);
        // std::cout << "number of swaps: " << swaps.size() << '\n';

        // // for (size_t i{0}; i < diffs.size(); ++i)
        // // {
        // //     std::cout << i << ' ' << diffs.at(i) << '\n';
        // // }

        // DiffMap diffMap {};
        // for (size_t i{0}; i < diffs.size(); ++i)
        // {
        //     diffMap[diffs[i]].push_back(swaps.at(i));
        // }
        // UniqueGateInt sortedDiffs {diffs.begin(), diffs.end()};
        // SwappedWires revSwaps {};
        // for (const GateInt& gateInt : sortedDiffs)
        // {
        //     std::copy(diffMap.at(gateInt).begin(), diffMap.at(gateInt).end(), std::back_inserter(revSwaps));
        // }

        // std::cout << "number of rev swaps: " << revSwaps.size() << '\n';

        // for (const auto& [key, values] : diffMap)
        // {
        //     std::cout << key << ' ' << values.size() << '\n';
        // }

        // long long lowest {-1};
        // for (size_t a{0}; a < revSwaps.size(); ++a)
        // {
        //     std::cout << '\t' << a << '\n';
        //     for (size_t b{a + 1}; b < revSwaps.size(); ++b)
        //     {
        //         UniqueWires ab {revSwaps.at(a).first, revSwaps.at(a).second, revSwaps.at(b).first, revSwaps.at(b).second};
        //         if (ab.size() < 4)
        //             continue;
        //         std::cout << "\t\t" << b << '\n';
        //         for (size_t c{b + 1}; c < revSwaps.size(); ++c)
        //         {
        //             UniqueWires abc {revSwaps.at(a).first, revSwaps.at(a).second, revSwaps.at(b).first, revSwaps.at(b).second, revSwaps.at(c).first, revSwaps.at(c).second};
        //             if (abc.size() < 6)
        //                 continue;
        //             // std::cout << "\t\t\t" << c << '\n';
        //             for (size_t d{c + 1}; d < revSwaps.size(); ++d)
        //             {
        //                 UniqueWires abcd {revSwaps.at(a).first, revSwaps.at(a).second, revSwaps.at(b).first, revSwaps.at(b).second, revSwaps.at(c).first, revSwaps.at(c).second, revSwaps.at(d).first, revSwaps.at(d).second};
        //                 if (abcd.size() < 8)
        //                     continue;
                        
        //                 // std::cout << "\t\t\t\t" << d << '\n';
        //                 WireGraph copyGraph {graph};

        //                 copyGraph.swapNodes(revSwaps.at(0).first, revSwaps.at(0).second);
        //                 copyGraph.swapNodes(revSwaps.at(1).first, revSwaps.at(1).second);
        //                 copyGraph.swapNodes(revSwaps.at(2).first, revSwaps.at(2).second);
        //                 copyGraph.swapNodes(revSwaps.at(3).first, revSwaps.at(3).second);

        //                 copyGraph.updateOutputs(revSwaps.at(0).first);
        //                 copyGraph.updateOutputs(revSwaps.at(0).second);
        //                 copyGraph.updateOutputs(revSwaps.at(1).first);
        //                 copyGraph.updateOutputs(revSwaps.at(1).second);
        //                 copyGraph.updateOutputs(revSwaps.at(2).first);
        //                 copyGraph.updateOutputs(revSwaps.at(2).second);
        //                 copyGraph.updateOutputs(revSwaps.at(3).first);
        //                 copyGraph.updateOutputs(revSwaps.at(3).second);

        //                 GateInt swapXGraph {getGraphInt(copyGraph, xWires, powersOfTwo)};
        //                 GateInt swapYGraph {getGraphInt(copyGraph, yWires, powersOfTwo)};
        //                 GateInt swapZGraph {getGraphInt(copyGraph, zWires, powersOfTwo)};
        //                 GateInt swapXYGraph {swapXGraph + swapYGraph};

        //                 // std::cout << ((swapXGraph + swapYGraph) - swapZGraph) << '\n';

        //                 long long diff {std::llabs(swapXYGraph - swapZGraph)};
        //                 if (lowest == -1 || diff < lowest)
        //                 {
        //                     lowest = diff;
        //                     std::cout << "lowest: " << lowest << '\n';
        //                 }

        //                 if (swapXYGraph == swapZGraph)
        //                 {
        //                     std::string s;
        //                     s.append(indexToStrMap.at(revSwaps.at(0).first));
        //                     s.append(indexToStrMap.at(revSwaps.at(0).second));
        //                     s.append(indexToStrMap.at(revSwaps.at(1).first));
        //                     s.append(indexToStrMap.at(revSwaps.at(1).second));
        //                     s.append(indexToStrMap.at(revSwaps.at(2).first));
        //                     s.append(indexToStrMap.at(revSwaps.at(2).second));
        //                     s.append(indexToStrMap.at(revSwaps.at(3).first));
        //                     s.append(indexToStrMap.at(revSwaps.at(3).second));
        //                     return s;
        //                 }
                        
        //                 // std::vector<SwappedPair> perms {revSwaps.at(a), revSwaps.at(b), revSwaps.at(c), revSwaps.at(d)};
        //                 // std::sort(perms.begin(), perms.end());
        //                 // do
        //                 // {
        //                 //     WireGraph copyGraph {graph};

        //                 //     copyGraph.swapNodes(perms.at(0).first, perms.at(0).second);
        //                 //     copyGraph.swapNodes(perms.at(1).first, perms.at(1).second);
        //                 //     copyGraph.swapNodes(perms.at(2).first, perms.at(2).second);
        //                 //     copyGraph.swapNodes(perms.at(3).first, perms.at(3).second);

        //                 //     copyGraph.updateOutputs(perms.at(0).first);
        //                 //     copyGraph.updateOutputs(perms.at(0).second);
        //                 //     copyGraph.updateOutputs(perms.at(1).first);
        //                 //     copyGraph.updateOutputs(perms.at(1).second);
        //                 //     copyGraph.updateOutputs(perms.at(2).first);
        //                 //     copyGraph.updateOutputs(perms.at(2).second);
        //                 //     copyGraph.updateOutputs(perms.at(3).first);
        //                 //     copyGraph.updateOutputs(perms.at(3).second);

        //                 //     GateInt swapXGraph {getGraphInt(copyGraph, xWires, powersOfTwo)};
        //                 //     GateInt swapYGraph {getGraphInt(copyGraph, yWires, powersOfTwo)};
        //                 //     GateInt swapZGraph {getGraphInt(copyGraph, zWires, powersOfTwo)};
        //                 //     GateInt swapXYGraph {swapXGraph + swapYGraph};

        //                 //     // std::cout << ((swapXGraph + swapYGraph) - swapZGraph) << '\n';

        //                 //     long long diff {std::llabs(swapXYGraph - swapZGraph)};
        //                 //     if (lowest == -1 || diff < lowest)
        //                 //     {
        //                 //         lowest = diff;
        //                 //         std::cout << "lowest: " << lowest << '\n';
        //                 //     }

        //                 //     if (swapXYGraph == swapZGraph)
        //                 //     {
        //                 //         std::string s;
        //                 //         s.append(indexToStrMap.at(perms.at(0).first));
        //                 //         s.append(indexToStrMap.at(perms.at(0).second));
        //                 //         s.append(indexToStrMap.at(perms.at(1).first));
        //                 //         s.append(indexToStrMap.at(perms.at(1).second));
        //                 //         s.append(indexToStrMap.at(perms.at(2).first));
        //                 //         s.append(indexToStrMap.at(perms.at(2).second));
        //                 //         s.append(indexToStrMap.at(perms.at(3).first));
        //                 //         s.append(indexToStrMap.at(perms.at(3).second));
        //                 //         return s;
        //                 //     }
        //                 // }
        //                 // while (std::next_permutation(perms.begin(), perms.end()));
        //             }
        //         }
        //     }
        // }

        long long lowest {-1};
        for (size_t a{0}; a < swaps.size(); ++a)
        {
            // std::cout << '\t' << a << '\n';
            WireGraph aGraph {graph};
            aGraph.swapNodes(swaps.at(a).first, swaps.at(a).second);
            aGraph.updateOutputs(swaps.at(a).first);
            aGraph.updateOutputs(swaps.at(a).second);

            for (size_t b{a + 1}; b < swaps.size(); ++b)
            {
                UniqueWires ab {swaps.at(a).first, swaps.at(a).second, swaps.at(b).first, swaps.at(b).second};
                if (ab.size() < 4)
                    continue;
                // std::cout << "\t\t" << b << '\n';
                WireGraph abGraph {aGraph};
                abGraph.swapNodes(swaps.at(b).first, swaps.at(b).second);
                abGraph.updateOutputs(swaps.at(b).first);
                abGraph.updateOutputs(swaps.at(b).second);

                for (size_t c{b + 1}; c < swaps.size(); ++c)
                {
                    UniqueWires abc {swaps.at(a).first, swaps.at(a).second, swaps.at(b).first, swaps.at(b).second, swaps.at(c).first, swaps.at(c).second};
                    if (abc.size() < 6)
                        continue;
                    // std::cout << "\t\t\t" << c << '\n';
                    WireGraph abcGraph {abGraph};
                    abcGraph.swapNodes(swaps.at(c).first, swaps.at(c).second);
                    abcGraph.updateOutputs(swaps.at(c).first);
                    abcGraph.updateOutputs(swaps.at(c).second);

                    for (size_t d{c + 1}; d < swaps.size(); ++d)
                    {
                        UniqueWires abcd {swaps.at(a).first, swaps.at(a).second, swaps.at(b).first, swaps.at(b).second, swaps.at(c).first, swaps.at(c).second, swaps.at(d).first, swaps.at(d).second};
                        if (abcd.size() < 8)
                            continue;
                        
                        // std::cout << "\t\t\t\t" << d << '\n';
                        WireGraph abcdGraph {abcGraph};

                        // copyGraph.swapNodes(swaps.at(a).first, swaps.at(a).second);
                        // copyGraph.swapNodes(swaps.at(b).first, swaps.at(b).second);
                        // copyGraph.swapNodes(swaps.at(c).first, swaps.at(c).second);
                        abcdGraph.swapNodes(swaps.at(d).first, swaps.at(d).second);

                        // copyGraph.updateOutputs(swaps.at(a).first);
                        // copyGraph.updateOutputs(swaps.at(a).second);
                        // copyGraph.updateOutputs(swaps.at(b).first);
                        // copyGraph.updateOutputs(swaps.at(b).second);
                        // copyGraph.updateOutputs(swaps.at(c).first);
                        // copyGraph.updateOutputs(swaps.at(c).second);
                        abcdGraph.updateOutputs(swaps.at(d).first);
                        abcdGraph.updateOutputs(swaps.at(d).second);

                        GateInt swapXGraph {getGraphInt(abcdGraph, xWires, powersOfTwo)};
                        GateInt swapYGraph {getGraphInt(abcdGraph, yWires, powersOfTwo)};
                        GateInt swapZGraph {getGraphInt(abcdGraph, zWires, powersOfTwo)};
                        GateInt swapXYGraph {swapXGraph + swapYGraph};

                        // std::cout << ((swapXGraph + swapYGraph) - swapZGraph) << '\n';

                        long long diff {std::llabs(swapXYGraph - swapZGraph)};
                        if (lowest == -1 || diff < lowest)
                        {
                            lowest = diff;
                            std::cout << "lowest: " << lowest << '\n';
                        }

                        if (swapXYGraph == swapZGraph)
                        {
                            std::string s;
                            s.append(indexToStrMap.at(swaps.at(a).first));
                            s.append(indexToStrMap.at(swaps.at(a).second));
                            s.append(indexToStrMap.at(swaps.at(b).first));
                            s.append(indexToStrMap.at(swaps.at(b).second));
                            s.append(indexToStrMap.at(swaps.at(c).first));
                            s.append(indexToStrMap.at(swaps.at(c).second));
                            s.append(indexToStrMap.at(swaps.at(d).first));
                            s.append(indexToStrMap.at(swaps.at(d).second));
                            return s;
                        }
                    }
                }
            }
        }

        SwappedWires swappedWires {{1,2},{3,4},{5,6},{7,8}};
        return sortAndCombineWires(swappedWires, indexToStrMap);
    }
}

#endif // AOC24B_H

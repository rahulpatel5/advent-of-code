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
#include <cassert>
#include "WireGraph.h"

#include <iostream>

/*
re-use solution to part 1

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

IDEA TO TRY:
    set up a unidirectional graph with wires as vertices
    have edges point from inputs to outputs
    means multiple edges per vertex
    could then work backwards from outputs that are off
    and see which wires (vertices could change)
    could use weighting (int) to show if 1 or 0
    or might need to use that for AND/OR/XOR?
graph seems faster

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

this approach seems to be getting somewhere
the issue now is that the swapping and operating wasn't right
need to reset the system / start from a clean state before swap & operate
ISSUE TO FIX:
    fix swapWires() now that it uses cleanGraph (missing wires?)

have resolved most issues, including removing edges between nodes
i.e. forgot that inputs should no longer point to a swapped output
while I get a solution, this wasn't accepted
had to look up a hint indicating the initial config of x and y is wrong
i.e. add a check that the x and y values change
also need a check that swaps both have inputs or both have no inputs

still wrong
seen that the puzzle isn't about adding specific x and y values
it's about the system being set up to add any numbers correctly
might need to scrap the current code and start again from scratch

could try setting up a test with different x and y values
if the swaps meet the tests, then we can try that as a solution
leave a complete code re-work to another time (assuming the above works)
*/

namespace aoc24b
{
    std::vector<std::int16_t> origValues {};
    using GateInt = unsigned long long;
    using WireStr = std::string;
    using Wire = std::int16_t;

    using StrToIndex = std::map<WireStr, Wire>;
    using IndexToStr = std::map<Wire, WireStr>;

    using Wires = std::vector<Wire>;
    using UniqueWires = std::set<Wire>;
    using WireValue = std::int16_t;
    using WireMap = std::map<Wire, WireValue>;
    using UniqueWireStr = std::set<WireStr>;

    using Input = Wire;
    using Output = Wire;
    using Operator = Wire;
    using Gate = std::array<Wire, 4>;
    using Gates = std::vector<Gate>;

    // from looking at puzzle
    constexpr int bitSize {48};
    using Bits = std::bitset<bitSize>;
    using CheckList = std::vector<bool>;
    // swapping pairs
    using SwappedPair = std::pair<Wire, Wire>;
    using SwappedWires = std::vector<SwappedPair>;
    // sequence of 4 pairs of swapped wires
    using SwappedWireSeq = std::array<SwappedPair, 4>;

    using GateBits = std::vector<std::int16_t>;
    // associations between gates - values 1-2: inputs, value 3: operator
    using Associations = std::map<Output, std::array<Wire, 3>>;

    using Power = unsigned long long;
    using Powers = std::array<Power, bitSize>;

    using WireLayers = std::vector<Wires>;
    using DiffInt = long long;
    using GateIntPair = std::pair<GateInt, GateInt>;
    using GateIntPairs = std::vector<GateIntPair>;
    using UniqueGateInt = std::set<GateInt>;
    using GateInts = std::vector<GateInt>;

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
                {
                    continue;
                }
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

    GateInt getGateInt(const WireMap& wires, const Wires& keyWires, const Powers& powersOfTwo)
    {
        GateInt gateInt {0};
        for (size_t i{0}; i < keyWires.size(); ++i)
            gateInt += wires.at(keyWires.at(i)) * powersOfTwo.at(i);
        return gateInt;
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

    GateInt getGraphInt(const WireGraph& graph, const Wires& keyWires, const Powers& powersOfTwo)
    {
        GateInt gateInt {0};
        for (size_t i{0}; i < keyWires.size(); ++i)
        {
            gateInt += graph.getValue(keyWires.at(i)) * powersOfTwo.at(i);
        }
        return gateInt;
    }

    Wires getXYZWires(const GateBits& diffBits, const StrToIndex& strToIndexMap)
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
            }
        }
        return xyz;
    }

    GateIntPairs setupTestValues(const UniqueGateInt values)
    {
        GateIntPairs testValues {};
        GateInts tempValues {values.begin(), values.end()};
        for (size_t i{0}; i < tempValues.size(); ++i)
        {
            for (size_t j{i + 1}; j < tempValues.size(); ++j)
            {
                testValues.push_back({tempValues.at(i), tempValues.at(j)});
            }
        }
        return testValues;
    }

    void changeXYInGraph(WireGraph& graph, const GateIntPair& pair, const Wires& xWires, const Wires& yWires)
    {
        assert(xWires.size() == yWires.size() && "xWires and yWires are not the same size.\n");
        Bits xBits {pair.first};
        Bits yBits {pair.second};
        for (size_t i{0}; i < xWires.size(); ++i)
        {
            graph.setValue(xWires.at(i), xBits.test(i));
            graph.setValue(yWires.at(i), yBits.test(i));
        }
    }

    GateIntPair testSwaps(const WireGraph& graph, const Wires& xWires, const Wires& yWires, const Wires& zWires, const Powers& powersOfTwo, const GateIntPairs& testPairs)
    {
        WireGraph copyGraph {graph};
        for (const GateIntPair& pair : testPairs)
        {
            changeXYInGraph(copyGraph, pair, xWires, yWires);
            copyGraph.operateAll();

            GateInt testXGraph {getGraphInt(copyGraph, xWires, powersOfTwo)};
            GateInt testYGraph {getGraphInt(copyGraph, yWires, powersOfTwo)};
            GateInt testZGraph {getGraphInt(copyGraph, zWires, powersOfTwo)};

            if (testXGraph + testYGraph != testZGraph)
            {
                std::cout << "fail: " << pair.first << ' ' << pair.second << '\n';
                return pair;
            }
            std::cout << "success: " << pair.first << ' ' << pair.second << '\n';
        }
        // if all swaps succeed, return a sentinel value
        // we'll use {1, 1}
        return {1, 1};
    }

    void swapWires(SwappedWires& swaps, const WireLayers& layers, const WireGraph& graph)
    {
        for (size_t a{0}; a < layers.size(); ++a)
        {
            for (size_t i{0}; i < layers.at(a).size(); ++i)
            {
                for (size_t b{a + 1}; b < layers.size(); ++b)
                {
                    for (size_t j{0}; j < layers.at(b).size(); ++j)
                    {
                        // no point swapping if values don't change
                        // we can't swap if one has no inputs
                        // we want swaps where both have inputs
                        // or where neither have inputs
                        if (((!graph.hasNoInput(layers.at(a).at(i)) && !graph.hasNoInput(layers.at(b).at(j))) || (graph.hasNoInput(layers.at(a).at(i)) && graph.hasNoInput(layers.at(b).at(j)))) && (graph.hasEmptyValue(layers.at(a).at(i)) || graph.hasEmptyValue(layers.at(b).at(j)) || graph.getValue(layers.at(a).at(i)) != graph.getValue(layers.at(b).at(j))))
                            swaps.push_back({layers.at(a).at(i), layers.at(b).at(j)});
                    }
                }
            }
        }
    }

    SwappedWires findSolutionSwaps(const SwappedWires& swaps, const WireGraph& graph, const Wires& xWires, const Wires& yWires, const Wires& zWires, const Powers& powersOfTwo, const GateInt& xyGraph, const GateInt& zGraph, const GateInt& initialXY, const GateIntPairs& testPairs)
    {
        // start by setting up a starting set of wires to swap
        SwappedWireSeq sequence {};
        sequence[0] = swaps.at(0);
        UniqueWires uniqueSeq {};
        uniqueSeq.insert(sequence[0].first);
        uniqueSeq.insert(sequence[0].second);
        for (size_t i{1}; i < 4; ++i)
        {
            SwappedPair next {};
            for (size_t j{1}; j < swaps.size(); ++j)
            {
                next = swaps.at(j);
                UniqueWires copyUnique {uniqueSeq};
                copyUnique.insert(next.first);
                copyUnique.insert(next.second);
                // make sure all wires are unique in the swaps
                if (copyUnique.size() == 2 * (i + 1))
                    break;
            }
            sequence[i] = next;
            uniqueSeq.insert(next.first);
            uniqueSeq.insert(next.second);
        }

        // set up what we'll use for the while loop
        size_t pairsIndex {0};
        size_t maxSwapSize {4};
        DiffInt bestMatch {std::llabs(xyGraph - zGraph)};
        // std::cout << "init best match " << bestMatch << '\n';

        // we want to change x and y values in graph later, so set up here
        WireGraph currentGraph {graph};

        size_t limit {10'000'000};
        size_t loop {0};
        int repeatCounter {0};
        size_t lastSwapIndex {};
        while (bestMatch != 0 && loop < limit)
        {
            // get swaps from set positions for this loop
            UniqueWires setPairs {};
            for (size_t p{0}; p < maxSwapSize; ++p)
            {
                if (p == pairsIndex)
                    continue;
                setPairs.insert(sequence.at(p).first);
                setPairs.insert(sequence.at(p).second);
            }

            SwappedPair currentBestPair {};
            // iterate all options and try swaps in current position (index)
            // alternative is to do a set number per loop e.g. 10
            for (size_t a{0}; a < swaps.size(); ++a)
            {
                UniqueWires copyPairs {setPairs};
                copyPairs.insert(swaps.at(a).first);
                copyPairs.insert(swaps.at(a).second);
                if (copyPairs.size() < 8)
                    continue;
                
                sequence[pairsIndex] = swaps.at(a);
                WireGraph copyGraph {currentGraph};
                for (const SwappedPair& pair : sequence)
                    copyGraph.swapNodes(pair.first, pair.second);
                copyGraph.operateAll();

                GateInt swapXGraph {getGraphInt(copyGraph, xWires, powersOfTwo)};
                GateInt swapYGraph {getGraphInt(copyGraph, yWires, powersOfTwo)};
                GateInt swapXYGraph {swapXGraph + swapYGraph};
                // if same as the original value, skip as this is wrong
                if (swapXYGraph == initialXY)
                    continue;
                GateInt swapZGraph {getGraphInt(copyGraph, zWires, powersOfTwo)};

                DiffInt diff {std::llabs(swapXYGraph - swapZGraph)};
                if (diff < bestMatch)
                {
                    bestMatch = diff;
                    currentBestPair = swaps.at(a);
                    lastSwapIndex = a;
                    std::cout << "lowest: " << bestMatch << '\n';
                    std::cout << "\tx, y: " << swapXGraph << ' ' << swapYGraph << '\n';
                    for (const SwappedPair& p : sequence)
                        std::cout << p.first << ' ' << p.second << ' ';
                    std::cout << '\n';
                }

                if (diff == 0)
                {
                    // test that these swaps work for other x and y values
                    GateIntPair testResult {testSwaps(copyGraph, xWires, yWires, zWires, powersOfTwo, testPairs)};
                    if (testResult.first == 1 && testResult.second == 1)
                        return {{sequence.at(0).first, sequence.at(0).second}, {sequence.at(1).first, sequence.at(1).second}, {sequence.at(2).first, sequence.at(2).second}, {sequence.at(3).first, sequence.at(3).second}};
                    // if a swap didn't work, use that in currentGraph
                    else
                    {
                        std::cout << "reached\n";
                        changeXYInGraph(currentGraph, testResult, xWires, yWires);
                        // need to reset bestMatch value
                        // we'll set to an arbitrarily high value
                        bestMatch = 1'000'000'000'000'000;
                    }
                }
            }
            // check if we got a good match and, if so, use that
            if (currentBestPair.first != 0 || currentBestPair.second != 0)
            {
                sequence[pairsIndex] = currentBestPair;
                repeatCounter = 0;
            }
            else
                ++repeatCounter;

            // to try to avoid an infinite loop
            if (repeatCounter == maxSwapSize)
            {
                UniqueWires repeatSwaps {};
                for (size_t p{0}; p < maxSwapSize; ++p)
                {
                    if (p == pairsIndex)
                        continue;
                    repeatSwaps.insert(sequence.at(p).first);
                    repeatSwaps.insert(sequence.at(p).second);
                }
                size_t repeatStartIndex {lastSwapIndex};
                SwappedPair replacement {};
                if (lastSwapIndex == swaps.size() - 1)
                    repeatStartIndex = 0;
                else
                    repeatStartIndex += 1;
                for (; repeatStartIndex < swaps.size(); ++repeatStartIndex)
                {
                    if (repeatSwaps.find(swaps.at(repeatStartIndex).first) == repeatSwaps.end() && repeatSwaps.find(swaps.at(repeatStartIndex).second) == repeatSwaps.end())
                    {
                        replacement = swaps.at(repeatStartIndex);
                        break;
                    }
                }
                if (replacement.first == replacement.second)
                {
                    for (size_t i{0}; i < repeatStartIndex; ++i)
                    {
                        if (repeatSwaps.find(swaps.at(i).first) == repeatSwaps.end() && repeatSwaps.find(swaps.at(i).second) == repeatSwaps.end())
                        {
                            replacement = swaps.at(i);
                            break;
                        }
                    }
                }
                sequence[pairsIndex] = replacement;
                repeatCounter = 0;
                // also reset bestMatch
                bestMatch = 1'000'000'000'000'000;
            }

            // shift index so that next position is covered in next loop
            ++pairsIndex;
            if (pairsIndex >= maxSwapSize)
                pairsIndex = 0;
            
            ++loop;
        }

        throw std::out_of_range("Did not find a solution from the available swaps.\n");
    }

    WireStr sortAndCombineWires(const SwappedWires& swapped, const IndexToStr& indexToStrMap)
    {
        UniqueWireStr wires {};
        for (const SwappedPair& pair : swapped)
        {
            wires.insert(indexToStrMap.at(pair.first));
            wires.insert(indexToStrMap.at(pair.second));
        }
        // hard-code check that this is 8 wires, as expected
        if (wires.size() != 8)
            throw std::invalid_argument("Final collection does not have the expected 8 wires.\n");

        bool isFirstLoop {true};
        WireStr final;
        for (const WireStr& wire : wires)
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
        // std::cout << "total: " << total << '\n';

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

        // get the initial x and y values as they're needed later
        GateInt initialX {getGateInt(wires, xWires, powersOfTwo)};
        GateInt initialY {getGateInt(wires, yWires, powersOfTwo)};
        GateInt initialXY {initialX + initialY};
        // std::cout << "initial: " << initialX << ' ' << initialY << ' ' << initialXY << '\n';

        // let's construct different x and y values to test with
        UniqueGateInt initTestValues {};
        initTestValues.insert(initialX);
        initTestValues.insert(initialY);
        // let's get the xor values too
        // we'll get max x/y value using info from the input
        Power maxXY {powersOfTwo.at(static_cast<GateInt>(xWires.size())) - 1};
        GateInt xorInitialX {initialX ^ maxXY};
        GateInt xorInitialY {initialY ^ maxXY};
        initTestValues.insert(xorInitialX);
        initTestValues.insert(xorInitialY);
        // chuck in 0 and the max x/y value
        initTestValues.insert(0);
        initTestValues.insert(maxXY);

        GateIntPairs testPairs {setupTestValues(initTestValues)};

        // create the graph class object
        WireGraph cleanGraph {total};
        graphSetup(cleanGraph, associations, wires);
        // create copy of graph to operate on
        WireGraph graph {cleanGraph};
        graph.operateAll();
        GateInt xGraph {getGraphInt(graph, xWires, powersOfTwo)};
        GateInt yGraph {getGraphInt(graph, yWires, powersOfTwo)};
        GateInt zGraph {getGraphInt(graph, zWires, powersOfTwo)};
        GateInt xyGraph {xGraph + yGraph};
        // std::cout << "orig: " << xyGraph << ' ' << zGraph << ' ' << std::llabs(xyGraph - zGraph) << '\n';

        // compare the sum of x and y vs z
        Bits addXY {xGraph + yGraph};
        Bits bitsOrigZGates {zGraph};
        Bits xorXYZ {addXY ^ bitsOrigZGates};
        // use this to find which bit positions (i.e. gates) differ
        GateBits diffBits {};
        for (size_t i{0}; i < bitSize; ++i)
        {
            if (xorXYZ.test(i))
                diffBits.push_back(i);
        }

        // // test
        // Wires testW { /* INTENTIONALLY EMPTY */ };
        // WireGraph try3 {cleanGraph};
        // try3.swapNodes(testW.at(0), testW.at(1));
        // try3.swapNodes(testW.at(2), testW.at(3));
        // try3.swapNodes(testW.at(4), testW.at(5));
        // try3.swapNodes(testW.at(6), testW.at(7));
        // try3.operateAll();
        // GateInt testx3 {getGraphInt(try3, xWires, powersOfTwo)};
        // GateInt testy3 {getGraphInt(try3, yWires, powersOfTwo)};
        // GateInt testz3 {getGraphInt(try3, zWires, powersOfTwo)};
        // GateInt testxy3 {testx3 + testy3};
        // std::cout << "here we are yet again result: " << testxy3 << ' ' << testz3 << ' ' << std::llabs(testxy3 - testz3) << '\n';

        // // test again
        // Gates copyGates {swapGates(gates, {{testW.at(0), testW.at(1)}, {testW.at(2), testW.at(3)}, {testW.at(4), testW.at(5)}, {testW.at(6), testW.at(7)}})};
        // WireMap copyWires {wires};
        // operateOnGates(copyGates, copyWires);
        // GateInt seeXGates {getGateInt(copyWires, xWires, powersOfTwo)};
        // GateInt seeYGates {getGateInt(copyWires, yWires, powersOfTwo)};
        // GateInt seeZGates {getGateInt(copyWires, zWires, powersOfTwo)};
        // GateInt seexygates {seeXGates + seeYGates};
        // std::cout << "final final test result: " << (seeXGates + seeYGates) << ' ' << seeZGates << ' ' << std::llabs((seeXGates + seeYGates) - seeZGates) << '\n';

        // get z wires associated with these different bits
        Wires possibleSwaps {getXYZWires(diffBits, strToIndexMap)};
        // std::cout << "number of possible swaps " << possibleSwaps.size() << '\n';

        // collect wires that act as inputs to these, i.e. candidate swaps
        WireLayers layers {};
        for (const Wire& swap : possibleSwaps)
        {
            Wires inputs {cleanGraph.getAllInputs(swap)};
            layers.push_back(inputs);
        }
        // std::cout << "(onion) layers: " << layers.size() << '\n';

        SwappedWires swaps {};
        swapWires(swaps, layers, cleanGraph);
        // std::cout << "swaps: " << swaps.size() << '\n';

        SwappedWires swappedWires {findSolutionSwaps(swaps, cleanGraph, xWires, yWires, zWires, powersOfTwo, xyGraph, zGraph, initialXY, testPairs)};

        // SwappedWires swappedWires {{1,2},{3,4},{5,6},{7,8}};
        return sortAndCombineWires(swappedWires, indexToStrMap);
    }
}

#endif // AOC24B_H

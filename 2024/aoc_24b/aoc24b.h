#ifndef AOC24B_H
#define AOC24B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>
#include <iterator>
#include "WireGraph.h"

#include <iostream>

/*
re-doing this puzzle from scratch as the old approach wasn't working
seen some hints that this puzzle involves bit adding and checking this

we can see that x and y wires are the ultimate inputs for z wires
this is obvious since x and y wires are the only ones with initial values
all z wires are based on all x and y wires as inputs, with one exception

presumably there are routes from one set of x/y wires to their z wire
and another route from earlier x/y wires to that z wire
difficult to prove this without developing a visualisation for graphs
(put on the issue list)

we know x + y == z
so, we know XOR would generate the right z output on a per bit basis
checking the input, that doesn't seem to always occur
so need to understand the logic behind where it doesn't

need to follow the logic for carry/left over from previous bit
if there is an AND connection for the previous x and y wires
this would give the correct carry/left over
but the carry/left over chains so not as simple as checking two places

e.g. x00 ^ y00 = z00 is good
looks like x## .. y## = intermediate wire, which then goes into z##
what are ways this could chain and how to identify where it's gone wrong?
e.g.
    x[n-1] ^ y[n-1] -> abc, e.g. 1 ^ 1 = 0
    x[n-1] & y[n-1] -> def, e.g. 1 & 1 = 1 (only carry over if both 1)
    x[n] ^ y[n] -> ghi, e.g. 1 ^ 0 = 1
    ghi ^ def -> z[n], e.g. 1 ^ 1 = 0

there are only 4 permutations for each pair of x and y wires
i.e. [0, 0], [1, 0], [0, 1], [1, 1]
if we can set up the connections, we could test each and check if working
would also need to account for carry over, but could add values for those?

remember that the last bit only has carry over from the previous x and y

logic is a bit complex. It seems to be:
- z00 should have only x and y inputs, with XOR operator
- last z should have only inputs to give carry over from penultimate z
- in between, z should have XOR operator and two inputs, with each being:
    - the current x and y inputs, with XOR operator
    - the carry over from previous x and y inputs, with OR operator
- the carry over uses an AND operator, with previous x and y inputs
where this isn't the case, suggests we need a swap
*/

namespace aoc24b
{
    using Wire = std::int16_t;
    using WireStr = std::string;
    using WireValue = std::int16_t;

    using StrToIndex = std::map<WireStr, Wire>;
    using IndexToStr = std::map<Wire, WireStr>;

    using Wires = std::vector<Wire>;
    using WireMap = std::map<Wire, WireValue>;
    using UniqueWireStr = std::set<WireStr>;
    using WireValues = std::vector<WireValue>;
    using WireInputs = std::pair<Wire, Wire>;

    using Input = Wire;
    using Output = Wire;
    using Operator = Wire;
    using Gate = std::array<Wire, 4>;
    using Gates = std::vector<Gate>;

    // from looking at puzzle
    constexpr int bitSize {46};
    // swapping pairs
    using SwappedPair = std::pair<Wire, Wire>;
    using SwappedWires = std::vector<SwappedPair>;

    // associations between gates - values 1-2: inputs, value 3: operator
    using Associations = std::map<Output, std::array<Wire, 3>>;

    // set AND, OR, and XOR operators to variables
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

    WireValue performOperation(const Gate& gate, const WireMap& wires)
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

    // consider collecting more info
    // e.g. splitting so we know where we have ANDs when we want OR etc
    Wires findWrongAddingAreas(WireGraph& graph, const Wires& xWires, const Wires& yWires, const Wires& zWires)
    {
        Wires wrong {};
        for (size_t i{0}; i < zWires.size(); ++i)
        {
            Wire zOper {graph.getOper(zWires.at(i))};
            // get the inputs to the z wire
            WireInputs initialZInputs {graph.getInputs(zWires.at(i))};

            // handle first bit differently as no carry over to handle
            if (i == 0)
            {
                if (zOper != -operXOR)
                {
                    std::cout << "0 " << i << ' ' << zWires.at(i) << '\n';
                    wrong.push_back(zWires.at(i));
                }
            }
            // handle last z bit differently as no direct x and y bits
            else if (i == zWires.size() - 1)
            {
                // OR or XOR would handle final bit fine
                // only issue is if operation is AND
                // might need to check if there are intermediate steps
                // let's skip that for now
                if (zOper == -operAND)
                {
                    // CONSIDER HERE
                    // can we link directly to the swap? (from the inputs)
                    std::cout << "1 " << i << ' ' << zWires.at(i) << '\n';
                    wrong.push_back(zWires.at(i));
                }
            }
            else
            {
                // if z wire operator is not XOR, wrong
                if (zOper != -operXOR)
                {
                    std::cout << "2 " << i << ' ' << zWires.at(i) << '\n';
                    wrong.push_back(zWires.at(i));
                    continue;
                }

                // check if either of the inputs has no input itself
                // this means that something has gone wrong
                if (graph.hasNoInput(initialZInputs.first) || graph.hasNoInput(initialZInputs.second))
                {
                    std::cout << "3 " << i << ' ' << zWires.at(i) << '\n';
                    wrong.push_back(zWires.at(i));
                    continue;
                }

                // need to handle second bit differently as no OR
                if (i == 1)
                {
                    if (!((graph.getOper(initialZInputs.first) == -operXOR || graph.getOper(initialZInputs.second) == -operXOR) && (graph.getOper(initialZInputs.first) == -operAND || graph.getOper(initialZInputs.second) == -operAND)))
                    {
                        if ((graph.getOper(initialZInputs.first) == -operXOR && graph.getOper(initialZInputs.second) != -operAND) || (graph.getOper(initialZInputs.first) == -operAND && graph.getOper(initialZInputs.second) != -operXOR))
                        {
                            std::cout << "4a " << i << ' ' << initialZInputs.second << '\n';
                            wrong.push_back(initialZInputs.second);
                        }
                        else
                        {
                            std::cout << "4b " << i << ' ' << initialZInputs.first << '\n';
                            wrong.push_back(initialZInputs.first);
                        }
                        continue;
                    }
                    Wire zANDBranch {};
                    if (graph.getOper(initialZInputs.first) == -operAND)
                        zANDBranch = initialZInputs.first;
                    else
                        zANDBranch = initialZInputs.second;
                    // check that inputs are previous x/y wires
                    WireInputs zANDInputs {graph.getInputs(zANDBranch)};
                    if (!((zANDInputs.first == xWires.at(i - 1) || zANDInputs.second == xWires.at(i - 1)) && (zANDInputs.first == yWires.at(i - 1) || zANDInputs.second == yWires.at(i - 1))))
                    {
                        std::cout << "5 " << i << ' ' << zANDBranch << '\n';
                        wrong.push_back(zANDBranch);
                        continue;
                    }
                    continue;
                }

                // we need to handle the XOR and OR branches separately

                // first make sure there is one XOR and one OR branch
                if (!((graph.getOper(initialZInputs.first) == -operXOR || graph.getOper(initialZInputs.second) == -operXOR) && (graph.getOper(initialZInputs.first) == -operOR || graph.getOper(initialZInputs.second) == -operOR)))
                {
                    if ((graph.getOper(initialZInputs.first) == -operXOR && graph.getOper(initialZInputs.second) != -operOR) || (graph.getOper(initialZInputs.first) == -operOR && graph.getOper(initialZInputs.second) != -operXOR))
                    {
                        // CONSIDER HERE
                        // could get swap via same inputs?
                        std::cout << "6a " << i << ' ' << initialZInputs.second << '\n';
                        wrong.push_back(initialZInputs.second);
                    }
                    else
                    {
                        std::cout << "6b " << i << ' ' << initialZInputs.first << '\n';
                        wrong.push_back(initialZInputs.first);
                    }
                    continue;
                }

                Wire zXORBranch {};
                Wire zORBranch {};
                if (graph.getOper(initialZInputs.first) == -operXOR)
                {
                    zXORBranch = initialZInputs.first;
                    zORBranch = initialZInputs.second;
                }
                else
                {
                    zXORBranch = initialZInputs.second;
                    zORBranch = initialZInputs.first;
                }

                // the XOR branch should point to the current x/y wires
                WireInputs expectedXYInputs {graph.getInputs(zXORBranch)};
                if (!((expectedXYInputs.first == xWires.at(i) || expectedXYInputs.second == xWires.at(i)) && (expectedXYInputs.first == yWires.at(i) || expectedXYInputs.second == yWires.at(i))))
                {
                    std::cout << "7 " << i << ' ' << zXORBranch << '\n';
                    wrong.push_back(zXORBranch);
                    continue;
                }

                // the OR branch should have an input to prev x/y wires
                WireInputs inputsForORBranch {graph.getInputs(zORBranch)};
                WireInputs furtherInputs1 {graph.getInputs(inputsForORBranch.first)};
                WireInputs furtherInputs2 {graph.getInputs(inputsForORBranch.second)};
                // we'll only check for x wires, for simplicity
                if (!(furtherInputs1.first == xWires.at(i - 1) || furtherInputs1.second == xWires.at(i - 1) || furtherInputs2.first == xWires.at(i - 1) || furtherInputs2.second == xWires.at(i - 1)))
                {
                    WireInputs furtherBeyondInputs {graph.getInputs(furtherInputs1.first)};
                    Wires beyondOutputs {graph.getOutputs(furtherBeyondInputs.first)};
                    if (std::find(beyondOutputs.begin(), beyondOutputs.end(), zWires.at(i - 1)) != beyondOutputs.end())
                    {
                        std::cout << "8a " << i << ' ' << furtherInputs1.second << '\n';
                        wrong.push_back(furtherInputs1.second);
                    }
                    else
                    {
                        std::cout << "8b " << i << ' ' << furtherInputs1.first << '\n';
                        wrong.push_back(furtherInputs1.first);
                    }
                }
            }
        }
        return wrong;
    }

    SwappedWires sortSwapsInOrder(const Wires& swaps, WireGraph& graph, const Wires& xWires, const Wires& yWires, const Wires& zWires)
    {
        assert(swaps.size() % 2 == 0 && "There are an odd number of swaps to order.\n");
        SwappedWires order {};
        for (size_t i{0}; i < swaps.size(); ++i)
        {
            // we'll skip if we get a z wire
            if (std::find(zWires.begin(), zWires.end(), swaps.at(i)) != zWires.end())
                continue;
            
            Wires current {swaps.at(i)};
            Wire matchingXWire {-1};
            while (matchingXWire < 0)
            {
                Wires next {};
                for (Wire c : current)
                {
                    WireInputs newInputs {graph.getInputs(c)};
                    if (std::find(xWires.begin(), xWires.end(), newInputs.first) != xWires.end())
                    {
                        matchingXWire = newInputs.first;
                        // std::cout << "1 " << newInputs.first << '\n';
                        break;
                    }
                    else
                        next.push_back(newInputs.first);
                    if (std::find(xWires.begin(), xWires.end(), newInputs.second) != xWires.end())
                    {
                        matchingXWire = newInputs.second;
                        // std::cout << "2 " << newInputs.second << '\n';
                        break;
                    }
                    else
                        next.push_back(newInputs.second);
                }
                current = next;
            }
            auto iteratorXWire {std::find(xWires.begin(), xWires.end(), matchingXWire)};
            size_t indexXWire {static_cast<size_t>(std::distance(xWires.begin(), iteratorXWire))};
            Wire matchingZWire {zWires.at(indexXWire)};
            // std::cout << "z: " << indexXWire << ' ' << zWires.at(indexXWire) << '\n';
            if (std::find(swaps.begin(), swaps.end(), matchingZWire) != swaps.end())
                order.push_back({swaps.at(i), matchingZWire});
            else
                throw std::out_of_range("Did not find valid matching z wire for swap pair.\n");
        }
        assert(order.size() == swaps.size() / 2 && "The number of paired swaps is not half the original number as expected.\n");
        return order;
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
    std::string parseAndGetSwappedWires(const std::array<std::string_view, N>& lines)
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
        // main effect of below function is to populate above variables
        // we collect the total number of unique wires as a side effect
        Wire total {addWiresAndGates<N>(wires, gates, associations, lines, strToIndexMap, indexToStrMap)};

        // we can save time by collecting x, y and z wires
        Wires xWires {};
        Wires yWires {};
        Wires zWires {};
        collectXYZWires(xWires, yWires, zWires, strToIndexMap);

        // create the graph class object
        WireGraph graph {total};
        graphSetup(graph, associations, wires);

        Wires wrongAdding {findWrongAddingAreas(graph, xWires, yWires, zWires)};
        for (const Wire& w : wrongAdding)
            std::cout << w << " (" << indexToStrMap.at(w) << ") ";
        std::cout << '\n';

        // swap wires, then test again
        // THIS IS WRONG - get swaps directly in findWrongAddingAreas()
        SwappedWires swaps {sortSwapsInOrder(wrongAdding, graph, xWires, yWires, zWires)};
        WireGraph revGraph {graph};
        for (const SwappedPair& p : swaps)
            revGraph.swapNodes(p.first, p.second);
        Wires testWrongness {findWrongAddingAreas(revGraph, xWires, yWires, zWires)};
        for (const Wire& w : testWrongness)
            std::cout << w << ' ';
        std::cout << '\n';
        // return sortAndCombineWires(swaps, indexToStrMap);

        SwappedWires swappedWires {{1,2},{3,4},{5,6},{7,8}};
        return sortAndCombineWires(swappedWires, indexToStrMap);
    }
}

#endif // AOC24B_H

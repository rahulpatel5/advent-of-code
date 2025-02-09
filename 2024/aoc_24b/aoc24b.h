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
// #include <iterator>
#include "WireGraph.h"

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

can be more methodical about this:
- for first bit, check x and y XOR into z, and store carry over wire
- get x and y XOR wire and check it XORs with carry over into z
- get x and y AND wire and rest of wires giving get new carry over wire
- repeat for all bits except last
- for last bit, z is the carry over

PSEUDOCODE:
    For each bit, from least to most significant
        If last bit
            If penultimate carryover isn't z, swap
        If first bit
            If x and y to z isn't XOR (and carryover not AND), swap
        // for other bits
        If xyXOR != XOR input to z, swap
        If x and y directly input to z
            If xyXOR == z, swap
            If xyAND == z, swap
        If z operation not XOR, swap
        If z inputs not xyXOR and carryover, swap
        If intermedCarryover not have xyXOR and carryover inputs, swap
        If next carryover input != output of non-z carryover, swap
        If nextCarryover inputs not intermedCarryover and xyAND, swap
*/

namespace aoc24b
{
    using Wire = std::int16_t;
    using WireStr = std::string;
    using WireValue = std::int16_t;

    using StrToIndex = std::map<WireStr, Wire>;
    using IndexToStr = std::map<Wire, WireStr>;

    using Wires = std::vector<Wire>;
    using UniqueWires = std::set<Wire>;
    using WireMap = std::map<Wire, WireValue>;
    using UniqueWireStr = std::set<WireStr>;
    using WireInputs = std::pair<Wire, Wire>;

    using Input = Wire;
    using Output = Wire;
    using Outputs = std::vector<Wire>;
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

    SwappedWires findErrorsInBinaryAdding(const WireGraph& origGraph, const Wires& xWires, const Wires& yWires, const Wires& zWires, const IndexToStr& indexToStrMap)
    {
        WireGraph graph {origGraph};
        SwappedWires swaps {};
        UniqueWires foundSwaps {};
        Wire carryover {};
        for (size_t i{0}; i < zWires.size(); ++i)
        {
            // the last bit should have a carry over that is z and with OR
            if (i == zWires.size() - 1)
            {
                if ((carryover != zWires.back() || graph.getOper(carryover) != -operOR) && foundSwaps.find(carryover) == foundSwaps.end())
                {
                    swaps.push_back({carryover, zWires.at(i)});
                    // for completeness
                    foundSwaps.insert(carryover);
                    foundSwaps.insert(zWires.at(i));
                    graph.swapNodes(carryover, zWires.at(i));
                }
                continue;
            }

            // we're swapping outputs so we'll assume all x and y are ok
            Wires xyOutputs {graph.getOutputs(xWires.at(i))};
            // if we're in the first bit, there's less to do
            if (i == 0)
            {
                Wire zWire {};
                for (Wire output : xyOutputs)
                {
                    if (output == zWires.at(i))
                        zWire = output;
                    else
                        carryover = output;
                }
                // if the first z has the wrong operator
                // assume it's on the other output
                if (graph.getOper(zWire) != -operXOR)
                {
                    swaps.push_back({zWire, carryover});
                    foundSwaps.insert(zWire);
                    foundSwaps.insert(carryover);
                    graph.swapNodes(carryover, zWire);
                    carryover = zWire;
                }
                // we'll skip checking if carryover has the right operator
                // there's no other options for swapping for the first bit
                continue;
            }

            // now we're in any of the bits between the first and last
            Wire xyXOR {};
            Wire xyAND {};
            Wire trueXYAND {};
            for (Wire output : xyOutputs)
            {
                if (graph.getOper(output) == -operXOR)
                    xyXOR = output;
                else if (graph.getOper(output) == -operAND)
                {
                    xyAND = output;
                    trueXYAND = xyAND;
                }
                // can't be any other operator
                else
                    throw std::invalid_argument("Error: x and y wires have an operator other than XOR or AND.\n");
            }

            // check that the z wire doesn't connect to x and y directly
            // to find the swap position we need to use carryover
            Wire trueZWire {zWires.at(i)};
            Outputs carryoverOutputs {graph.getOutputs(carryover)};
            Output carryoverOutputXOR {};
            for (Wire output : carryoverOutputs)
            {
                if (graph.getOper(output) == -operXOR)
                    carryoverOutputXOR = output;
            }

            // we know the same wire can't swap twice
            if (xyXOR == zWires.at(i) && foundSwaps.find(zWires.at(i)) == foundSwaps.end())
            {
                swaps.push_back({carryoverOutputXOR, zWires.at(i)});
                foundSwaps.insert(carryoverOutputXOR);
                foundSwaps.insert(zWires.at(i));
                graph.swapNodes(carryoverOutputXOR, zWires.at(i));
                // trueZWire = carryoverOutputXOR;
            }
            // if xyAND is out of position, we assume xyXOR is in position
            // the XOR output of xyXOR will point to what z swaps with
            else if (xyAND == zWires.at(i) && foundSwaps.find(zWires.at(i)) == foundSwaps.end())
            {
                Wire zSwap {};
                for (Wire output : graph.getOutputs(xyXOR))
                {
                    if (graph.getOper(output) == -operXOR)
                        zSwap = output;
                }
                swaps.push_back({zSwap, zWires.at(i)});
                foundSwaps.insert(zSwap);
                foundSwaps.insert(zWires.at(i));
                graph.swapNodes(zSwap, zWires.at(i));
                // trueZWire = zSwap;
                trueXYAND = zSwap;
            }
            // check if z doesn't have XOR operator
            else if (graph.getOper(zWires.at(i)) != -operXOR && foundSwaps.find(zWires.at(i)) == foundSwaps.end())
            {
                // we'll follow the XOR output of xyXOR to get the swap
                Wire zSwap {};
                for (Wire output : graph.getOutputs(xyXOR))
                {
                    if (graph.getOper(output) == -operXOR)
                        zSwap = output;
                }
                swaps.push_back({zSwap, zWires.at(i)});
                foundSwaps.insert(zSwap);
                foundSwaps.insert(zWires.at(i));
                graph.swapNodes(zSwap, zWires.at(i));
            }

            // let's check that the right wires are pointing to z wire
            // we'll use the corrected z wire
            WireInputs zXORInputs {graph.getInputs(trueZWire)};

            // first check if the z wire is in the right position
            if (zXORInputs.first != xyXOR && zXORInputs.second != xyXOR && zXORInputs.first != xyAND && zXORInputs.second != xyAND)
            {
                // find the XOR output from xyXOR
                bool foundXOR {false};
                for (Wire output : graph.getOutputs(xyXOR))
                {
                    if (graph.getOper(output) == -operXOR && foundSwaps.find(zWires.at(i)) == foundSwaps.end())
                    {
                        swaps.push_back({output, zWires.at(i)});
                        foundSwaps.insert(output);
                        foundSwaps.insert(zWires.at(i));
                        graph.swapNodes(output, zWires.at(i));
                        foundXOR = true;
                    }
                    // suppose we should check xyAND in case it's the input
                    if (!foundXOR)
                    {
                        for (Wire output : graph.getOutputs(xyAND))
                        {
                            if (graph.getOper(output) == -operXOR && foundSwaps.find(zWires.at(i)) == foundSwaps.end())
                            {
                                swaps.push_back({output, zWires.at(i)});
                                foundSwaps.insert(output);
                                foundSwaps.insert(zWires.at(i));
                                graph.swapNodes(output, zWires.at(i));
                            }
                        }
                    }
                }
            }
            
            // if xyAND points to the z wire instead of xyOR, swap
            // use sentinel value
            Wire zInputXOR {-1};
            if (graph.getOper(zXORInputs.first) == -operXOR)
                zInputXOR = zXORInputs.first;
            else if (graph.getOper(zXORInputs.second) == -operXOR)
                zInputXOR = zXORInputs.second;
            
            if (zXORInputs.first != xyXOR && zXORInputs.second != xyXOR && (zXORInputs.first == xyAND || zXORInputs.second == xyAND) && foundSwaps.find(xyXOR) == foundSwaps.end())
            {
                swaps.push_back({xyXOR, xyAND});
                foundSwaps.insert(xyXOR);
                foundSwaps.insert(xyAND);
                graph.swapNodes(xyXOR, xyAND);
                trueXYAND = xyXOR;
            }
            // check if z input with XOR operator connects to x and y
            // if not, they need to swap
            else if (zInputXOR != xyXOR && foundSwaps.find(zInputXOR) == foundSwaps.end())
            {
                swaps.push_back({zInputXOR, xyXOR});
                foundSwaps.insert(zInputXOR);
                foundSwaps.insert(xyXOR);
                graph.swapNodes(xyXOR, zInputXOR);
            }

            // the other z input has to be the previous carryover
            Wire zInputOR {-1};
            if (graph.getOper(zXORInputs.first) == -operOR)
                zInputOR = zXORInputs.first;
            else if (graph.getOper(zXORInputs.second) == -operOR)
                zInputOR = zXORInputs.second;

            if ((zXORInputs.second == zInputXOR || zXORInputs.second == xyXOR) && zXORInputs.first != carryover && foundSwaps.find(carryover) == foundSwaps.end())
            {
                swaps.push_back({zXORInputs.first, carryover});
                foundSwaps.insert(zXORInputs.first);
                foundSwaps.insert(carryover);
                graph.swapNodes(zXORInputs.first, carryover);
            }
            else if ((zXORInputs.first == zInputXOR || zXORInputs.first == xyXOR) && zXORInputs.second != carryover && foundSwaps.find(carryover) == foundSwaps.end())
            {
                swaps.push_back({zXORInputs.second, carryover});
                foundSwaps.insert(zXORInputs.second);
                foundSwaps.insert(carryover);
                graph.swapNodes(zXORInputs.second, carryover);
            }

            // finally we need to handle the (next) carry over
            // collect the wires we need

            // we need the right xyAND, so correct it
            xyAND = trueXYAND;

            carryoverOutputs = graph.getOutputs(carryover);
            assert(carryoverOutputs.size() == 2 && "Error: the carryover does not have only 2 outputs.\n");
            Wire intermediateCarryoverUp {-1};
            for (Wire output : carryoverOutputs)
            {
                if (output != trueZWire)
                    intermediateCarryoverUp = output;
            }

            Wires xyANDOutputs {graph.getOutputs(xyAND)};
            assert(xyANDOutputs.size() == 1 && "Error: the x and y AND branch does not have only one output.\n");
            Wire possibleNextCarryover {xyANDOutputs.at(0)};
            Wire candidateCarryover {possibleNextCarryover};

            WireInputs possNextCarryoverInputs {graph.getInputs(possibleNextCarryover)};
            Wire intermediateCarryoverDown {-1};
            if (possNextCarryoverInputs.first == xyAND)
                intermediateCarryoverDown = possNextCarryoverInputs.second;
            else if (possNextCarryoverInputs.second == xyAND)
                intermediateCarryoverDown = possNextCarryoverInputs.first;
            
            // if the intermediates don't match, swap
            // if the inputs to intermediateUP are xyXOR and carryover,
            // that's correct and the next carryover may be wrong
            WireInputs intermedCarryDownInputs {graph.getInputs(intermediateCarryoverDown)};
            if (intermediateCarryoverDown != intermediateCarryoverUp && (intermedCarryDownInputs.first != xyXOR || intermedCarryDownInputs.second != xyXOR))
            {
                if (foundSwaps.find(intermediateCarryoverDown) == foundSwaps.end())
                {
                    swaps.push_back({intermediateCarryoverDown, intermediateCarryoverUp});
                    foundSwaps.insert(intermediateCarryoverDown);
                    foundSwaps.insert(intermediateCarryoverUp);
                    graph.swapNodes(intermediateCarryoverDown, intermediateCarryoverUp);
                }
            }
            // otherwise the issue will be with next carryover
            else if (intermediateCarryoverDown != intermediateCarryoverUp)
            {
                Wires intermedCarryUpOutputs {graph.getOutputs(intermediateCarryoverUp)};
                assert(intermedCarryUpOutputs.size() == 1 && "Error: expected there to be only one output from intermediate carryover.\n");
                Wire intermedCarryUpOutput {intermedCarryUpOutputs.at(0)};
                if (intermedCarryUpOutput != possibleNextCarryover && foundSwaps.find(intermedCarryUpOutput) == foundSwaps.end())
                {
                    swaps.push_back({intermedCarryUpOutput, possibleNextCarryover});
                    foundSwaps.insert(intermedCarryUpOutput);
                    foundSwaps.insert(possibleNextCarryover);
                    graph.swapNodes(intermedCarryUpOutput, possibleNextCarryover);
                }
                candidateCarryover = intermedCarryUpOutput;
            }

            // finally set the new carryover
            carryover = candidateCarryover;
        }
        return swaps;
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

        SwappedWires swaps {findErrorsInBinaryAdding(graph, xWires, yWires, zWires, indexToStrMap)};

        // swap wires, then test again
        WireGraph revGraph {graph};
        for (const SwappedPair& p : swaps)
            revGraph.swapNodes(p.first, p.second);

        SwappedWires testWrongness {findErrorsInBinaryAdding(revGraph, xWires, yWires, zWires, indexToStrMap)};
        return sortAndCombineWires(swaps, indexToStrMap);
    }
}

#endif // AOC24B_H

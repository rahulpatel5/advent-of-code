#ifndef AOC24A_H
#define AOC24A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <bitset>
#include <stdexcept>

/*
read wire values in map
read gate connections (in vector?)
traverse connections one at a time, updating or adding to map as needed
at end, get values for z keys
*/

namespace aoc24a
{
    using GateInt = unsigned long long;
    using Wire = std::string;
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

    GateInt getFinalFigure(const WireMap& wires)
    {
        Bits binary {};
        for (int i{0}; i < bitSize; ++i)
        {
            Wire key {"z"};
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

    template <std::size_t N>
    GateInt parseAndGetWireOutput(const std::array<std::string_view, N>& lines)
    {
        WireMap wires {};
        Gates gates {};
        addWiresAndGates<N>(wires, gates, lines);
        operateOnGates(gates, wires);
        return getFinalFigure(wires);
    }
}

#endif // AOC24A_H

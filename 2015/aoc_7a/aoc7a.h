#ifndef AOC7A_H
#define AOC7A_H

#include "parse_string.h"
#include "bitwise.h"
#include <sstream>
#include <unordered_map>
#include <vector>

namespace aoc7a
{
    using StringVectorMap = std::unordered_map<std::string, std::vector<std::string>>;

    StringVectorMap getInstructions(const std::string& input)
    {
        std::istringstream passage {input};
        std::string line;
        StringVectorMap wires{};
        while (std::getline(passage, line))
        {
            parse_string::writeInstructions(line, wires);
        }
        return wires;
    }

    bool isGateInCircuit(std::string_view wire, const bitwise::StringIntMap& circuit)
    {
        for (const auto& element : circuit)
        {
            if (element.first == wire)
                return true;
        }
        return false;
    }

    bool canConvertToInteg(const std::string& ulCandidate)
    {
        try
        {
            static_cast<bitwise::integ>(std::stoul(ulCandidate));
            return true;
        }
        catch(const std::invalid_argument& e)
        {
            return false;
        }
    }

    bitwise::StringIntMap getCircuit(const StringVectorMap& wires)
    {
        bitwise::StringIntMap circuit{};

        for (const auto& [endGate, vector] : wires)
        {
            if ((vector.size() == 1) && canConvertToInteg(vector[0]))
                circuit[endGate] = std::stoi(vector[0]);
        }

        // is there a way to perform this whole function in one loop?
        while (circuit.size() < wires.size())
        {
            for (const auto& [endGate, vector] : wires)
            {
                if (isGateInCircuit(endGate, circuit))
                    continue;
                
                else if ((vector.size() == 1) && !canConvertToInteg(vector[0]) && isGateInCircuit(vector[0], circuit))
                    circuit[endGate] = circuit.at(vector[0]);

                else if ((vector.size() == 2) && isGateInCircuit(vector[1], circuit))
                    circuit[endGate] = bitwise::notOperation(circuit, vector[1]);
                
                else if (((vector[1] == "LSHIFT") || (vector[1] == "RSHIFT")) && isGateInCircuit(vector[0], circuit))
                    circuit[endGate] = bitwise::shiftOperation(circuit, vector);
                
                else if ((vector[1] == "AND") && isGateInCircuit(vector[0], circuit) && isGateInCircuit(vector[2], circuit))
                    circuit[endGate] = bitwise::andOperation(circuit, vector[0], vector[2]);
                
                else if ((vector[1] == "AND") && canConvertToInteg(vector[0]) && isGateInCircuit(vector[2], circuit))
                    circuit[endGate] = bitwise::andOperationWithDigit(circuit, vector[0], vector[2]);
                
                else if ((vector[1] == "OR") && isGateInCircuit(vector[0], circuit) && isGateInCircuit(vector[2], circuit))
                    circuit[endGate] = bitwise::orOperation(circuit, vector[0], vector[2]);
            }
        }
        return circuit;
    }

    bitwise::StringIntMap parseWires(const std::string& input)
    {
        StringVectorMap wires {getInstructions(input)};
        bitwise::StringIntMap circuit {getCircuit(wires)};
        return circuit;
    }
}

#endif // AOC7A_H

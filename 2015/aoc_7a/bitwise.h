#ifndef BITWISE_H
#define BITWISE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

namespace bitwise
{
    using integ = unsigned short;
    using StringIntMap = std::unordered_map<std::string, integ>;

    integ notOperation(const StringIntMap& circuit, const std::string& wire)
    {
        return (~(circuit.at(wire)));
    }

    integ shiftOperation(const StringIntMap& circuit, const std::vector<std::string> vector)
    {
        integ shiftBit {static_cast<integ>(std::stoul(vector[2]))};
        if (vector[1] == "LSHIFT")
            return (circuit.at(vector[0]) << shiftBit);
        else if (vector[1] == "RSHIFT")
            return (circuit.at(vector[0]) >> shiftBit);
        else
            throw std::invalid_argument("Invalid operation was selected: " + vector[1]);
    }

    integ andOperation(const StringIntMap& circuit, const std::string& wire1, const std::string& wire2)
    {
        return ((circuit.at(wire1)) & (circuit.at(wire2)));
    }

    integ andOperationWithDigit(const StringIntMap& circuit, const std::string& wire1, const std::string& wire2)
    {
        integ decimal1 {static_cast<integ>(std::stoul(wire1))};
        return (decimal1 & (circuit.at(wire2)));
    }

    integ orOperation(const StringIntMap& circuit, const std::string& wire1, const std::string& wire2)
    {
        return ((circuit.at(wire1)) | (circuit.at(wire2)));
    }
}

#endif // BITWISE_H

#ifndef PARSE_STRING_H
#define PARSE_STRING_H

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace parse_string
{
    // uses split function at: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    std::vector<std::string> splitString(const std::string& s, char delim)
    {
        std::vector<std::string> result{};
        std::stringstream ss {s};
        std::string item;
        while (std::getline(ss, item, delim))
        {
            result.push_back(item);
        }
        return result;
    }

    void writeInstructions(std::string_view line, std::unordered_map<std::string, std::vector<std::string>>& wires)
    {
        size_t arrow {line.find('>')};
        size_t lastSpace {line.find_first_of(' ', arrow)};
        std::string endGate {line.substr(lastSpace + 1)};

        size_t preArrowSpace {line.find_last_of(' ', arrow)};
        std::string leftSide {line.substr(0, preArrowSpace)};
        std::vector<std::string> instruction {splitString(leftSide, ' ')};

        wires[endGate] = instruction;
    }
}

#endif // PARSE_STRING_H

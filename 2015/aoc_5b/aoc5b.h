#ifndef AOC5B_H
#define AOC5B_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace aoc5b
{
    bool isDoubleLetterPair(char c, char d)
    {
        return (c == d);
    }

    std::string joinChars(char c, char d)
    {
        return {std::string() + c + d};
    }

    bool meetProperties(int pairCount, int pairTarget, int repeatCount, int repeatTarget)
    {
        return ((pairCount >= pairTarget) && (repeatCount >= repeatTarget));
    }

    std::vector<int> getIndexOfPair(const std::string& pair, const std::string& line)
    {
        std::vector<int> indices{};
        for (auto index{0}; index < line.size(); ++index)
        {
            if (index < (line.size() - 1))
            {
                if (pair == (std::string() + line[index] + line[index + 1]))
                    indices.push_back(index);
            }
        }
        return indices;
    }

    bool isValidRepeat(const std::string& pair, const std::string& line)
    {
        std::vector<int> repeatIndex {getIndexOfPair(pair, line)};
        for (auto index{1}; index < repeatIndex.size(); ++index)
        {
            if ((repeatIndex[index] - repeatIndex[0]) > 1)
                return true;
        }
        return false;
    }

    int countValidRepeats(std::unordered_map<std::string, int> map, const std::string& line)
    {
        int count{0};
        for (const auto& [key, value] : map)
        {
            if (value > 1)
                count += isValidRepeat(key, line);
        }
        return count;
    }

    bool checkStringProperties(const std::string& line, int pairTarget, int repeatTarget)
    {
        int pairCount{0};
        std::unordered_map<std::string, int> mapRepeats{};
        for (auto index{0}; index < line.size(); ++index)
        {
            if (index < line.size() - 2)
                pairCount += isDoubleLetterPair(line[index], line[index + 2]);
            if (index < line.size() - 1)
                mapRepeats[joinChars(line[index], line[index + 1])] += 1;
        }

        int repeatCount {countValidRepeats(mapRepeats, line)};
        return (meetProperties(pairCount, pairTarget, repeatCount, repeatTarget));
    }

    int countNiceStrings(const std::string& input, int pairTarget, int repeatTarget)
    {
        std::istringstream passage {input};
        std::string line;
        int niceCount{0};

        while (std::getline(passage, line))
        {
            niceCount += checkStringProperties(line, pairTarget, repeatTarget);
        }
        return niceCount;
    }
}

#endif // AOC5B_H

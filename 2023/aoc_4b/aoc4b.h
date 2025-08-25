#ifndef AOC4B_H
#define AOC4B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <numeric>

/*
TRACK copies held of each scratchcard

FOR each card:
    READ winning numbers and scratch card numbers
    COUNT each winning number in scratch card section
    ADD 1 copy of next cards for each match, multiplied by copies of card

RETURN total count of scratch cards
*/

namespace aoc4b
{
    using Cards = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Number = int;
    using Numbers = std::vector<Number>;
    using CardMap = std::map<Cards, Cards>;

    template <std::size_t N>
    CardMap setupCardMap()
    {
        CardMap map {};
        for (Cards c{1}; c <= N; ++c)
        {
            map[c] = 1;
        }
        return map;
    }

    Numbers getNumbers(const std::string& line)
    {
        Numbers numbers {};
        size_t space {line.find(' ')};
        size_t index {space + 1};
        while (index != std::string::npos)
        {
            size_t nextSpace {line.find(' ', index + 1)};
            std::string numberStr {};
            if (nextSpace != std::string::npos)
                numberStr = line.substr(index, nextSpace - index);
            else
                numberStr = line.substr(index);
            if (numberStr.size() > 0)
                numbers.push_back(std::stoi(numberStr));

            if (nextSpace != std::string::npos)
                index = nextSpace + 1;
            else
                index = nextSpace;
        }
        return numbers;
    }

    void addCardCopies(const Numbers& card, const Numbers& winning, CardMap& cardMap, Cards cardCount)
    {
        Cards matches {0};
        for (Number num : card)
        {
            if (std::find(winning.begin(), winning.end(), num) != winning.end())
            {
                ++matches;
            }
        }

        for (Cards c{1}; c <= matches; ++c)
        {
            cardMap[cardCount + c] += cardMap.at(cardCount);
        }
    }

    Cards countFinalTotalCards(const CardMap& cards)
    {
        Cards total {0};
        for (const auto& c : cards)
        {
            total += c.second;
        }
        return total;
    }

    template <std::size_t L>
    Cards getTotalWinningCards(const Lines<L>& lines)
    {
        CardMap cards {setupCardMap<L>()};

        Cards count {1};
        for (std::string_view line : lines)
        {
            std::string fullLine {std::string(line)};
            size_t colon {fullLine.find(':')};
            size_t pipe {fullLine.find('|')};

            Numbers winningNumbers {getNumbers(fullLine.substr(colon + 1, pipe - (colon + 1)))};
            Numbers scratchcard {getNumbers(fullLine.substr(pipe + 1))};
            addCardCopies(scratchcard, winningNumbers, cards, count);

            ++count;
        }

        return countFinalTotalCards(cards);
    }
}

#endif // AOC4B_H

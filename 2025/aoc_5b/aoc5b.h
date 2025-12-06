#ifndef AOC5B_H
#define AOC5B_H

#include <array>
#include <string_view>
#include <vector>
#include <utility>
#include <map>
#include <stdexcept>

/*
FOR each ingredient range:
    IF range not present:
        ADD to list
    ELSE IF extension of an existing range:
        EXTEND that part of the list

COMBINE final ranges
RETURN sum of lengths of final ranges
*/

namespace aoc5b
{
    using IngredientInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Range = std::pair<IngredientInt, IngredientInt>;
    using Ranges = std::vector<Range>;
    using Ingredients = std::vector<IngredientInt>;
    using RangeMap = std::map<IngredientInt, Ingredients>;

    template <std::size_t L>
    Ranges getInputRanges(const Lines<L>& lines)
    {
        Ranges ranges {};
        for (std::string_view line : lines)
        {
            if (line.find('-') == std::string_view::npos)
                continue;
            
            std::string rangeStr { std::string(line) };
            size_t hyphen { rangeStr.find('-') };
            std::string firstNumStr { rangeStr.substr(0, hyphen) };
            std::string secondNumStr { rangeStr.substr(hyphen + 1) };
            ranges.push_back({std::stoll(firstNumStr), std::stoll(secondNumStr)});
        }
        return ranges;
    }

    RangeMap sortRangesIntoMap(const Ranges& ranges)
    {
        RangeMap map {};
        for (const Range& range : ranges)
        {
            if (map.find(range.first) == map.end() || (map.find(range.first) != map.end() && range.second > map.at(range.first).back()))
                map[range.first].push_back(range.second);
        }
        return map;
    }

    void combineRanges(RangeMap& map)
    {
        IngredientInt lastLowValue {-2};
        IngredientInt lastHighValue {-2};
        for (auto it {map.cbegin()}; it != map.cend(); )
        {
            if (it->first > lastHighValue + 1)
            {
                lastLowValue = it->first;
                lastHighValue = it->second.back();
                ++it;
            }

            else if (it->first <= lastHighValue + 1)
            {
                if (it->second.back() > lastHighValue)
                {
                    map[lastLowValue].push_back(it->second.back());
                    lastHighValue = it->second.back();
                }
                map.erase(it++);
            }

            else
                throw std::out_of_range("Expected ranges to overlap. But they don't.\n");
        }
    }

    IngredientInt numbersInFinalRanges(const RangeMap& map)
    {
        IngredientInt count {0};
        for (const auto& [k, v] : map)
        {
            count += (v.back() + 1) - k;
        }
        return count;
    }

    template <std::size_t L>
    IngredientInt countAllIngredients(const Lines<L>& lines)
    {
        Ranges inputRanges { getInputRanges<L>(lines) };

        // sorting using a map
        RangeMap sortedRangeMap { sortRangesIntoMap(inputRanges) };
        combineRanges(sortedRangeMap);
        return numbersInFinalRanges(sortedRangeMap);
    }
}

#endif // AOC5B_H

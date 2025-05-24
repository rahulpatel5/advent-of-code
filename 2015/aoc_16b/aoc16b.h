#ifndef AOC16B_H
#define AOC16B_H

#include <array>
#include <string_view>
#include <vector>
#include <string>
#include <map>
#include <cassert>

/*
*IF condition NOT in range list, is present and matches value
    THEN pass test
    MOVE to next test
*ELSE IF condition is cat or tree, is present and over value
    THEN pass test
    MOVE to next test
*ELSE IF condition is pom or goldfish, is present and under value
    THEN pass test
    MOVE to next test
ELSE
    FAIL test
    MOVE to next element to test

RUN test for all elements
IF only one element full passes
    PRINT element
ELSE
    RAISE error that too many matches found

lines starting with * are changes from part 1 solution
*/

namespace aoc16b
{
    using AuntInt = int;
    using TrueAunts = std::vector<AuntInt>;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Property = std::string;
    using Value = int;
    using PropertyInfo = std::pair<Property, Value>;
    using PropertyMap = std::map<Property, Value>;
    using Aunts = std::vector<PropertyMap>;

    PropertyInfo readProperty(const Property& line)
    {
        size_t colon {line.find(':')};
        Property property {line.substr(0, colon)};
        Value value {std::stoi(line.substr(colon + 2))};
        return {property, value};
    }

    template <std::size_t N>
    PropertyMap readTrueInfo(const Lines<N>& input)
    {
        PropertyMap map {};
        for (std::string_view row : input)
        {
            PropertyInfo propertyInfo {readProperty(std::string(row))};
            map[propertyInfo.first] = propertyInfo.second;
        }
        return map;
    }

    AuntInt readAuntNumber(const Property& line)
    {
        size_t space {line.find(' ')};
        return std::stoi(line.substr(space + 1));
    }

    bool failCondition(const PropertyInfo& propertyInfo, const PropertyMap& trueAuntMap)
    {
        if (trueAuntMap.find(propertyInfo.first) == trueAuntMap.end())
            return false;
        
        // pass if number of cats or trees is over the value
        if (propertyInfo.first == "cats" || propertyInfo.first == "trees")
        {
            if (propertyInfo.second > trueAuntMap.at(propertyInfo.first))
                return false;
            else
                return true;
        }
        // pass if number of poms or goldfish is under the value
        else if (propertyInfo.first == "pomeranians" || propertyInfo.first == "goldfish")
        {
            if (propertyInfo.second < trueAuntMap.at(propertyInfo.first))
                return false;
            else
                return true;
        }
        else
        {
            if (propertyInfo.second == trueAuntMap.at(propertyInfo.first))
                return false;
            else
                return true;
        }
    }

    template <std::size_t N>
    AuntInt findGiftingAunt(const Lines<N>& auntList, const PropertyMap& trueAuntMap)
    {
        TrueAunts trueAunts {};

        for (std::string_view line : auntList)
        {
            std::string row {std::string(line)};
            size_t colon {row.find(':')};
            AuntInt auntNo {readAuntNumber(row.substr(0, colon))};

            size_t comma {std::move(colon)};
            bool auntMatch {true};
            while (comma != std::string::npos)
            {
                size_t nextComma {row.find(',', comma + 1)};
                PropertyInfo propertyInfo {readProperty(row.substr(comma + 2, nextComma))};
                if (failCondition(propertyInfo, trueAuntMap))
                {
                    auntMatch = false;
                    break;
                }

                comma = std::move(nextComma);
            }
            if (auntMatch)
                trueAunts.push_back(auntNo);
        }

        assert(trueAunts.size() == 1 && "Matched several Aunts Sue when only one was expected.\n");
        return trueAunts.at(0);
    }

    template <std::size_t TA, std::size_t L>
    AuntInt findAunt(const Lines<TA>& trueAuntInfo, const Lines<L>& auntList)
    {
        PropertyMap trueAuntMap {readTrueInfo<TA>(trueAuntInfo)};

        return findGiftingAunt<L>(auntList, trueAuntMap);
    }
}

#endif // AOC16B_H

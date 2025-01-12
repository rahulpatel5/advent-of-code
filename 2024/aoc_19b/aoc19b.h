#ifndef AOC19B_H
#define AOC19B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include <map>

/*
re-use solution for part 1
easy switch to change returning true to counting all successful matches
the issue is this will likely massively increase the time for the code to run
try this first and then think how to make this more efficient

need to collect what towels have been used and in what order
otherwise can miss some arrangements

as expected, takes far too long to check each arrangement
could try something like memoisation for repeat patterns?
may be better to try one arrangement at a time (so memoisation is useful)?

making each combination from towels causes lots of duplicates
it will likely be more efficient to go through the design letter by letter
and count how many ways to reach a certain point
e.g. 3 ways to get to bgr, e.g. bg, r or b, gr or b, g, r
by collecting (in a map?) the count of reaching a point
we can more simply count combinations of reaching later points
*/

namespace aoc19b
{
    using Towel = std::string;
    using Towels = std::set<Towel>;
    using Design = std::string;
    using Designs = std::set<Design>;
    using TowelSizes = std::pair<size_t, size_t>;
    using TowelBySizeMap = std::map<size_t, Designs>;
    using MatchInt = long long;
    using DesignMatchMap = std::map<Design, MatchInt>;

    template <std::size_t N>
    void getTowelsAndDesigns(Towels& towels, Designs& designs, TowelBySizeMap& towelsBySize, const std::array<std::string_view, N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.find(',') != std::string_view::npos)
            {
                size_t nextComma {line.find(',')};
                Towel firstTowel {line.substr(0, nextComma)};
                towels.insert(firstTowel);
                towelsBySize[firstTowel.size()].insert(firstTowel);
                size_t currComma {nextComma + 1};
                while (nextComma != std::string_view::npos)
                {
                    nextComma = line.find(',', currComma);
                    Towel towel {line.substr(currComma + 1, nextComma - (currComma + 1))};
                    towels.insert(towel);
                    towelsBySize[towel.size()].insert(towel);
                    currComma = nextComma + 1;
                }
            }
            else if (line.size() != 0)
                designs.insert(std::string(line));
        }
    }

    TowelSizes getMinMaxTowelSizes(const Towels& towels)
    {
        size_t min {};
        size_t max {};
        for (const Towel& towel : towels)
        {
            if (min == 0 || towel.size() < min)
                min = towel.size();
            if (max == 0 || towel.size() > max)
                max = towel.size();
        }
        return {min, max};
    }

    MatchInt howManyTimesIsDesignPossible(const Design& design, const Towels& towels, const TowelBySizeMap& towelsBySize, const TowelSizes& towelSizesMinMax)
    {
        // collect the different ways to reach a particular design
        DesignMatchMap matches {};
        // iterate through each letter of the design
        for (size_t i{1}; i <= design.size(); ++i)
        {
            // the design we want to match in this loop
            Design currentDesign {design.substr(0, i)};

            // iterate through towels to add
            for (size_t j{towelSizesMinMax.first}; j <= towelSizesMinMax.second; ++j)
            {
                // check if a towel matches the current size and design
                if (j == i && towelsBySize.at(j).find(currentDesign) != towelsBySize.at(j).end())
                    matches[currentDesign] += 1;
                
                // now check if there are combinations we can make
                if (i > j)
                {
                    Design previous {design.substr(0, i - j)};
                    Design next {design.substr(i - j, j)};
                    if (matches.find(previous) != matches.end() && towelsBySize.at(j).find(next) != towelsBySize.at(j).end())
                        matches[currentDesign] += matches.at(previous);
                }
            }
        }
        if (matches.find(design) != matches.end())
            return matches.at(design);
        else
            return 0;
    }

    MatchInt countPossibleDesigns(const Designs& designs, const Towels& towels, const TowelBySizeMap& towelsBySize, const TowelSizes& towelSizesMinMax)
    {
        MatchInt success {0};
        for (const Design& design : designs)
        {
            success += howManyTimesIsDesignPossible(design, towels, towelsBySize, towelSizesMinMax);
        }
        return success;
    }

    template <std::size_t N>
    MatchInt parseAndCountDesigns(const std::array<std::string_view, N>& lines)
    {
        Towels towels {};
        Designs designs {};
        TowelBySizeMap towelsBySize {};
        getTowelsAndDesigns<N>(towels, designs, towelsBySize, lines);
        TowelSizes towelSizesMinMax {getMinMaxTowelSizes(towels)};
        return countPossibleDesigns(designs, towels, towelsBySize, towelSizesMinMax);
    }
}

#endif // AOC19B_H

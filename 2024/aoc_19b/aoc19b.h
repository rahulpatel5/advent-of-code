#ifndef AOC19B_H
#define AOC19B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include "List.h"

#include <iostream>

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
*/

namespace aoc19b
{
    using Towel = std::string;
    using Towels = std::set<Towel>;
    using Design = std::string;
    using Designs = std::set<Design>;
    using TowelSizes = std::pair<size_t, size_t>;
    using Arrangement = std::vector<Design>;
    using Arrangements = std::vector<Arrangement>;
    using UniqueArrangements = std::set<Arrangement>;
    using Index = size_t;
    using Indexes = std::vector<Index>;

    template <std::size_t N>
    void getTowelsAndDesigns(Towels& towels, Designs& designs, const std::array<std::string_view, N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.find(',') != std::string_view::npos)
            {
                size_t nextComma {line.find(',')};
                Towel firstTowel {line.substr(0, nextComma)};
                towels.insert(firstTowel);
                size_t currComma {nextComma + 1};
                while (nextComma != std::string_view::npos)
                {
                    nextComma = line.find(',', currComma);
                    Towel towel {line.substr(currComma + 1, nextComma - (currComma + 1))};
                    towels.insert(towel);
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

    int howManyTimesIsDesignPossible(const Design& design, const Towels& towels, const TowelSizes& towelSizesMinMax)
    {
        // std::cout << "0\n";
        List validArrangements {};
        // Arrangements validArrangements {};
        int counter {0};
        // use min and max towel sizes to determine for loop
        // if we start with the largest towels, it may save time
        for (size_t initial{towelSizesMinMax.second}; initial >= towelSizesMinMax.first; --initial)
        {
            Design start {design.substr(0, initial)};
            // std::cout << "start, " << start.size() << '\n';
            // if this design doesn't exist, skip
            if (towels.find(start) == towels.end())
                continue;
            // check we haven't created the design already (somehow)
            if (start == design)
            {
                validArrangements.add({start});
                continue;
                // std::cout << "\tstart matches design " << counter << '\n';
            }
            // if (design == "rrbgbr")
            //     std::cout << "\t\tgoing for rrbgbr, with start " << start << '\n';
            
            List possibleDesigns {};
            possibleDesigns.push({start});

            while (possibleDesigns.size() > 0)
            {
                // std::cout << "\tloop " << possibleDesigns.size() << '\n';
                Arrangement designsToAdd {};
                List newToAdd {};
                Indexes indexToDelete {};
                UniqueArrangements uniqueLoopDesigns {};
                for (size_t i{0}; i < possibleDesigns.size(); ++i)
                {
                    // std::cout << "3\n";
                    bool firstMatch {true};
                    for (size_t next{towelSizesMinMax.first}; next <= towelSizesMinMax.second; ++next)
                    {
                        size_t strSoFarSize {possibleDesigns.str_size(i)};
                        // std::cout << "4\n";
                        // if we're going above the design size, stop
                        if (strSoFarSize + next > design.size())
                            break;
                        // std::cout << "5\n";
                        // check if next substring exists
                        Design nextString {design.substr(strSoFarSize, next)};
                        if (towels.find(nextString) == towels.end())
                            continue;
                        // if we're repeating previous arrays, skip
                        Arrangement newArrangement {possibleDesigns[i]};
                        newArrangement.push_back(nextString);
                        if (uniqueLoopDesigns.find(newArrangement) != uniqueLoopDesigns.end())
                            continue;
                            
                        // we get here if it does exist
                        uniqueLoopDesigns.insert(newArrangement);
                        // check if we've got the target design
                        if (strSoFarSize + nextString.size() == design.size() && nextString == design.substr(strSoFarSize, nextString.size()))
                        {
                            // std::cout << "7a\n";
                            validArrangements.push({newArrangement});
                        }
                        else if (firstMatch)
                        {
                            // std::cout << "7b, " << designsToAdd.size() << '\n';
                            designsToAdd.push_back(nextString);
                            // std::cout << "7bi, " << designsToAdd.size() << '\n';
                            firstMatch = false;
                        }
                        else
                        {
                            // std::cout << "7c\n";
                            newToAdd.push(newArrangement);
                        }
                    }
                    // std::cout << "8\n";
                    if (firstMatch)
                    {
                        // std::cout << "8a, to del: " << i << '\n';
                        indexToDelete.push_back(i);
                    }
                }
                // std::cout << "9a: " << indexToDelete.size() << ' ' << possibleDesigns.size() << '\n';
                // now do some post loop clean up
                possibleDesigns.trim(indexToDelete);
                // std::cout << "9b: " << designsToAdd.size() << ' ' << possibleDesigns.size() << '\n';
                possibleDesigns.add(designsToAdd);
                // std::cout << "9c: " << newToAdd.size() << ' ' << possibleDesigns.size() << '\n';
                possibleDesigns.push_many(newToAdd);
                // std::cout << "9d: " << possibleDesigns.size() << '\n';
            }
        }
        // std::cout << "10\n";
        UniqueArrangements unique {validArrangements.begin(), validArrangements.end()};
        return static_cast<int>(unique.size());
    }

    int countPossibleDesigns(const Designs& designs, const Towels& towels, const TowelSizes& towelSizesMinMax)
    {
        int success {0};
        std::cout << "going thru: " << designs.size() << '\n';
        for (const Design& design : designs)
        {
            std::cout << "for: " << design << ' ' << design.size() << '\n';
            success += howManyTimesIsDesignPossible(design, towels, towelSizesMinMax);
        }
        return success;
    }

    template <std::size_t N>
    int parseAndCountDesigns(const std::array<std::string_view, N>& lines)
    {
        Towels towels {};
        Designs designs {};
        getTowelsAndDesigns<N>(towels, designs, lines);
        TowelSizes towelSizesMinMax {getMinMaxTowelSizes(towels)};
        return countPossibleDesigns(designs, towels, towelSizesMinMax);
    }
}

#endif // AOC19B_H

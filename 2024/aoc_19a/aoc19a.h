#ifndef AOC19A_H
#define AOC19A_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>

/*
get available designs
get desired designs
iterate through each combination of available designs
question of whether to do this by string combination or using vectors
can check that size of combo matches size of target, to save time
return true if any match the desired design, otherwise design is impossible
count and return the number of possible desired designs

tricky bit is getting towels in any order
can use next_permutation with vector to handle that

could use memoisation to collect patterns and designs
this may allow counting where a design is possible

instead of collecting designs, break up target design e.g. 'r' 'gr' 'grg'
keep trying to find the towels for that break down until it fails
then move onto the next one, and keep going until more than the max length
*/

namespace aoc19a
{
    using Towel = std::string;
    using Towels = std::set<Towel>;
    using Design = std::string;
    using Designs = std::set<Design>;
    using TowelSizes = std::pair<size_t, size_t>;

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

    bool isDesignPossible(const Design& design, Towels& towels, TowelSizes& towelSizesMinMax)
    {
        // short circuit to answer if design already exists (somehow)
        if (towels.find(design) != towels.end())
            return true;
        
        // use min and max towel sizes to determine for loop
        // if we start with the largest towels, it may save time
        for (size_t initial{towelSizesMinMax.second}; initial >= towelSizesMinMax.first; --initial)
        {
            Design start {design.substr(0, initial)};
            // if this design doesn't exist, skip
            if (towels.find(start) == towels.end())
                continue;
            // check we haven't created the design already (somehow)
            if (start == design)
                return true;
            
            Designs possibleDesigns {};
            possibleDesigns.insert(start);
            while (possibleDesigns.size() > 0)
            {
                Designs nextPossDesigns {};
                for (const Design& possDesign : possibleDesigns)
                {
                    for (size_t next{towelSizesMinMax.first}; next <= towelSizesMinMax.second; ++next)
                    {
                        // if we're going above the design size, stop
                        if (possDesign.size() + next > design.size())
                            break;
                        // check if next substring exists
                        Design nextString {design.substr(possDesign.size(), next)};
                        if (towels.find(nextString) == towels.end())
                            continue;
                        // if it does exist, add to next designs to check
                        Design nextPoss {possDesign};
                        nextPoss.append(nextString);
                        nextPossDesigns.insert(nextPoss);
                        if (nextPoss.size() > towelSizesMinMax.second)
                            towelSizesMinMax.second = nextPoss.size();
                        towels.insert(nextPoss);
                        // check if we've got the target design
                        if (nextPoss == design)
                            return true;
                    }
                }
                possibleDesigns = nextPossDesigns;
            }
        }
        return false;
    }

    int countPossibleDesigns(const Designs& designs, Towels& towels, TowelSizes& towelSizesMinMax)
    {
        int success {0};
        for (const Design& design : designs)
        {
            if (isDesignPossible(design, towels, towelSizesMinMax))
                ++success;
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

#endif // AOC19A_H

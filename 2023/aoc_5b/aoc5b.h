#ifndef AOC5B_H
#define AOC5B_H

#include <array>
#include <string_view>
#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>

/*
READ seed ranges
READ map for each category

FOR each lowest location value from 0:
    USE map data to find corresponding value for each category, in reverse
    IF destination value exists:
        USE corresponding source value
    ELSE
        USE the destination value as the source value
    GET the initial seed value
    IF the initial seed value is in the seed range:
        RETURN the lowest location value
*/

namespace aoc5b
{
    using SeedInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Seed = long long;
    using Seeds = std::vector<Seed>;
    using Map = std::array<Seed, 3>;
    using Maps = std::vector<Map>;

    template <std::size_t N>
    Seeds getSeeds(const Lines<N>& lines)
    {
        // first get the seed numbers
        Seeds seeds {};
        for (std::string_view line : lines)
        {
            if (line.find("seeds:") != std::string::npos)
            {
                std::string info {std::string(line)};
                size_t colon {info.find(':')};
                size_t index {colon + 2};
                while (index != std::string::npos)
                {
                    size_t space {info.find(' ', index)};
                    std::string numberStr {info.substr(index, space - index)};
                    seeds.push_back(std::stoll(numberStr));
                    // avoid overflow if at end of string
                    index = space + ((space == std::string::npos) ? 0 : 1);
                }
            }
        }
        return seeds;
    }

    Map getMapInfo(const std::string& line)
    {
        Map map {};
        size_t index {0};
        size_t arrayIndex {0};
        while (index != std::string::npos)
        {
            size_t space {line.find(' ', index)};
            std::string numberStr {line.substr(index, space - index)};
            map[arrayIndex] = std::stoll(numberStr);

            index = space + ((space == std::string::npos) ? 0 : 1);
            ++arrayIndex;
        }
        return map;
    }

    template <std::size_t N>
    Maps getMaps(const Lines<N>& lines, const std::string& text)
    {
        Maps maps {};
        bool foundText {false};
        for (std::string_view line : lines)
        {
            if (foundText && (line.find_first_not_of(" \t\n") == std::string::npos || &line == &lines.back()))
                break;

            if (line.find(text) != std::string::npos)
            {
                foundText = true;
                continue;
            }
            else if (foundText)
            {
                Map map {getMapInfo(std::string(line))};
                maps.push_back(map);
            }
        }
        return maps;
    }

    Seed getSourceValue(Seed seed, const Maps& sourceToDestMaps)
    {
        for (const Map& map : sourceToDestMaps)
        {
            if (seed >= map.at(0) && seed < map.at(0) + map.at(2))
            {
                return (map.at(1) + (seed - map.at(0)));
            }
        }

        // if don't match above, then return the original (source) value
        return seed;
    }

    bool seedIsInValidRange(Seed seed, const Seeds& seedRange)
    {
        for (Seed s{0}; s < seedRange.size(); s += 2)
        {
            if ((seed >= seedRange.at(s)) && (seed < seedRange.at(s) + seedRange.at(s + 1)))
                return true;
        }
        return false;
    }

    template <std::size_t L>
    SeedInt getLowestLocationNumber(const Lines<L>& lines)
    {
        Seeds initialSeeds {getSeeds<L>(lines)};
        Maps seedToSoil {getMaps<L>(lines, "seed-to-soil map:")};
        Maps soilToFert {getMaps<L>(lines, "soil-to-fertilizer map:")};
        Maps fertToWate {getMaps<L>(lines, "fertilizer-to-water map:")};
        Maps wateToLite {getMaps<L>(lines, "water-to-light map:")};
        Maps liteToTemp {getMaps<L>(lines, "light-to-temperature map:")};
        Maps tempToHumi {getMaps<L>(lines, "temperature-to-humidity map:")};
        Maps humiToLoca {getMaps<L>(lines, "humidity-to-location map:")};

        for (Seed loca{0}; ; ++loca)
        {
            // get source value for each category in reverse sequence
            Seed humi {getSourceValue(loca, humiToLoca)};
            Seed temp {getSourceValue(humi, tempToHumi)};
            Seed lite {getSourceValue(temp, liteToTemp)};
            Seed wate {getSourceValue(lite, wateToLite)};
            Seed fert {getSourceValue(wate, fertToWate)};
            Seed soil {getSourceValue(fert, soilToFert)};
            Seed seed {getSourceValue(soil, seedToSoil)};

            if (seedIsInValidRange(seed, initialSeeds))
                return loca;
        }

        throw std::out_of_range("Did not find a solution.\n");
    }
}

#endif // AOC5B_H

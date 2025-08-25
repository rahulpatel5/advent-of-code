#ifndef AOC5A_H
#define AOC5A_H

#include <array>
#include <string_view>
#include <vector>
#include <string>
#include <cassert>

/*
READ seeds to plant
READ map for each category
FOR each seed:
    USE the map data to find the corresponding value for each category
    IF source value exists:
        USE corresponding destination value
    ELSE
        USE the source value as the destination value
    STORE the final (location) value

RETURN the lowest location value out of all the seeds
*/

namespace aoc5a
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

    Seed getDestinationValue(Seed seed, const Maps& sourceToDestMaps)
    {
        for (const Map& map : sourceToDestMaps)
        {
            if (seed >= map.at(1) && seed < map.at(1) + map.at(2))
            {
                return (map.at(0) + (seed - map.at(1)));
            }
        }

        // if don't match above, then return the original (source) value
        return seed;
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

        SeedInt lowest {-1};

        for (Seed seed : initialSeeds)
        {
            // get destination value for each category in sequence
            Seed soil {getDestinationValue(seed, seedToSoil)};
            Seed fert {getDestinationValue(soil, soilToFert)};
            Seed wate {getDestinationValue(fert, fertToWate)};
            Seed lite {getDestinationValue(wate, wateToLite)};
            Seed temp {getDestinationValue(lite, liteToTemp)};
            Seed humi {getDestinationValue(temp, tempToHumi)};
            Seed loca {getDestinationValue(humi, humiToLoca)};

            if (lowest == -1 || loca < lowest)
                lowest = loca;
        }

        return lowest;
    }
}

#endif // AOC5A_H

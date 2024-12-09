#ifndef AOC9A_H
#define AOC9A_H

#include "parse.h"
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

#include <iostream>

/*

*/

namespace aoc9a
{
    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2>& pair) const
        {
            auto hash1 {std::hash<T1>{}(pair.first)};
            auto hash2 {std::hash<T2>{}(pair.second)};
            return hash1 ^ hash2;
        }
    };

    using TravelMap = std::unordered_map<std::pair<std::string, std::string>, int, pair_hash>;

    void updateTravelMap(TravelMap& brochure, const std::vector<std::string>& locations)
    {
        brochure[{locations[0], locations[1]}] = std::stoi(locations[2]);
    }

    template <typename T, int numberUniqueLocations>
    int getShortestRoute(const T& lines)
    {
        TravelMap brochure{};
        std::set<std::string> uniqueLocations {};

        // parse input to get location data
        for (const auto& line : lines)
        {
            std::vector<std::string> data {parse::readLocationData(line)};
            parse::addUniqueLocations(data, uniqueLocations);
            updateTravelMap(brochure, data);
        }

        // used parsed input to generate routes
        constexpr std::size_t countNonDupeJourneys {parse::countNonRepeatJourneys<std::size_t, numberUniqueLocations>()};

        return 0; // findShortestRoute(routes);
    }
}

#endif // AOC9A_H

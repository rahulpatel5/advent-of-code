#ifndef AOC9A_H
#define AOC9A_H

#include "parse.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

/*
get unique locations
use unique locations to set up array with all locations
use next_permutation to get all routes (note: inefficient as it includes duplicate routes)
iterate each route and find the shortest
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

    using TravelMap = std::map<std::pair<std::string, std::string>, int>;

    void updateTravelMap(TravelMap& brochure, const std::vector<std::string>& locations)
    {
        brochure[{locations[0], locations[1]}] = std::stoi(locations[2]);
    }

    std::set<std::vector<std::string>> setUpAllRoutes(const std::vector<std::string>& locations)
    {
        std::set<std::vector<std::string>> routes {};
        std::vector<std::string> eachLocation {locations};
        routes.insert(eachLocation);

        while (std::next_permutation(eachLocation.begin(), eachLocation.end()))
        {
            routes.insert(eachLocation);
        }

        return routes;
    }

    int getJourneyDistance(const std::string& location1, const std::string& location2, const TravelMap& brochure)
    {
        std::pair<std::string, std::string> key1 {location1, location2};
        std::pair<std::string, std::string> key2 {location2, location1};
        int distance {};

        if (brochure.find(key1) != brochure.end())
            distance = brochure.at(key1);
        else if (brochure.find(key2) != brochure.end())
            distance = brochure.at(key2);
        else
            throw std::invalid_argument("Location pair not found.");
        
        return distance;
    }

    int findShortestDistance(const std::set<std::vector<std::string>>& routes, const TravelMap& brochure)
    {
        int shortest {};
        bool firstLoop {true};

        for (const std::vector<std::string>& route : routes)
        {
            int distance {};
            for (size_t i{1}; i < route.size(); ++i)
            {
                std::string location1 {route[i - 1]};
                std::string location2 {route[i]};
                int locationDistance {getJourneyDistance(location1, location2, brochure)};

                distance += locationDistance;
                if (!firstLoop && distance >= shortest)
                    break;
            }
            if (firstLoop)
            {
                shortest = distance;
                firstLoop = false;
            }
            if (distance < shortest)
                shortest = distance;
        }

        return shortest;
    }

    template <typename T>
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

        // set up array to use with next_permutation
        std::vector<std::string> eachLocation {};
        for (const std::string& location : uniqueLocations)
            eachLocation.push_back(location);
        
        std::set<std::vector<std::string>> allRoutes {setUpAllRoutes(eachLocation)};

        return findShortestDistance(allRoutes, brochure);
    }
}

#endif // AOC9A_H

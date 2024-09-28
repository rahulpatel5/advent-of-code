#ifndef AOC9A_H
#define AOC9A_H

#include "parse.h"
#include <string>
#include <vector>
#include <set>
#include <map>

#include <iostream>

namespace aoc9a
{
    using TravelMap = std::map<std::pair<std::string, std::string>, int>;
    using RouteMap = std::map<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>;

    void updateTravelMap(TravelMap& brochure, const std::vector<std::string>& locations)
    {
        brochure[{locations[0], locations[1]}] = std::stoi(locations[2]);
    }

    RouteMap createRouteMapKeys(const std::set<std::string>& uniqueLocations)
    {
        RouteMap routes {};
        for (const auto& location : uniqueLocations)
        {
            for (auto i{0}; i < uniqueLocations.size(); ++i)
            {
                routes[{location, i}];
            }
        }
        return routes;
    }

    std::pair<std::size_t, int> checkAndUpdateIndex(std::pair<std::size_t, std::size_t> validIndex, const RouteMap& routes, std::string_view origin, std::string_view uniqueLocation, std::size_t keyIndex)
    {
        std::pair<std::string, int> key {uniqueLocation, keyIndex};
        if (routes.at(key).size() == 0)
        {
            return ((uniqueLocation == origin) && (0 < validIndex.second)) ? std::make_pair(keyIndex, static_cast<std::size_t>(0)) : validIndex;
        }
        else
        {
            return ((routes.at(key).back().first == origin) && (routes.at(key).size() < validIndex.second)) ? std::make_pair(keyIndex, std::size(routes.at(key))) : validIndex;
        }
    }

    bool isLocationInRoute(const RouteMap& routes, std::string_view newLocation, std::string_view uniqueLocation, int keyIndex)
    {
        std::pair<std::string, int> key {uniqueLocation, keyIndex};
        for (const auto& location : routes.at(key))
        {
            if (location.first == newLocation)
                return true;
        }
        return false;
    }

    std::size_t getFirstValidIndex(const RouteMap& routes, std::string_view origin, std::string_view destination, std::string_view uniqueLocation, int numberUniqueLocations)
    {
        std::pair<std::size_t, std::size_t> validIndex {numberUniqueLocations, numberUniqueLocations};
        for (std::size_t i{0}; i < numberUniqueLocations; ++i)
        {
            if (!isLocationInRoute(routes, destination, uniqueLocation, i))
            {
                validIndex = checkAndUpdateIndex(validIndex, routes, origin, uniqueLocation, i);
            }
        }
        return validIndex.first;
    }

    void addLocationToRoutes(RouteMap& routes, std::string_view origin, std::string_view destination, int distance, std::set<std::string> uniqueLocations)
    {
        int numberUniqueLocations {static_cast<int>(std::size(uniqueLocations))};

        for (const auto& uniqueLocation : uniqueLocations)
        {
            if (uniqueLocation == destination)
                continue;
            
            auto locationIndex {getFirstValidIndex(routes, origin, destination, uniqueLocation, numberUniqueLocations)};
            // default value for locationIndex is size of uniqueLocations
            // so do not have a valid index if that is the value
            if (locationIndex < numberUniqueLocations)
            {
                std::pair<std::string, int> newLocation {destination, distance};
                routes.at({uniqueLocation, locationIndex}).push_back(newLocation);
            }
        }
    }

    // likely too many for loops being used to be efficient
    // start here if wanting to improve performance
    void populateRoutes(RouteMap& routes, TravelMap brochure, std::set<std::string> uniqueLocations)
    {
        // loop for the number of unique locations there are
        // seems like this would guarantee fully populating routes
        // i.e. need iterations to capture the full number of connections
        for (auto i{0}; i < uniqueLocations.size(); ++i)
        {
            for (const auto& key_value : brochure)
            {
                // handle the first of the locations
                addLocationToRoutes(routes, key_value.first.first, key_value.first.second, key_value.second, uniqueLocations);

                // handle the second of the locations
                // i.e. journey could go in reverse
                addLocationToRoutes(routes, key_value.first.second, key_value.first.first, key_value.second, uniqueLocations);
            }
        }
    }

    int findShortestRoute(const RouteMap& routes)
    {
        int shortest {30'000}; // use arbitrarily large initial value
        for (const auto& [key, vector] : routes)
        {
            int distance {0};
            // get the distance of each route
            for (const auto& pair : vector)
            {
                distance += pair.second;
            }
            // if route is shortest so far, set it as distance
            if (distance < shortest)
                shortest = distance;
        }
        return shortest;
    }

    int getShortestRoute(const auto& lines)
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
        RouteMap routes {createRouteMapKeys(uniqueLocations)};
        populateRoutes(routes, brochure, uniqueLocations);

        return findShortestRoute(routes);
    }
}

#endif // AOC9A_H

#ifndef AOC8B_H
#define AOC8B_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <vector>
#include <ranges>
#include <stdexcept>
#include "Network.h"

/*
READ puzzle like in part 1

FOR each connection of shortest connection:
    IF all junctions in network:
        RETURN product of x coordinates of last junctions connected
*/

namespace aoc8b
{
    using BoxInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Coordinate = long long;
    using Junction = std::tuple<Coordinate, Coordinate, Coordinate>;
    using JunctionIndex = long long;
    using JunctionMap = std::map<JunctionIndex, Junction>;
    using Junctions = std::vector<JunctionIndex>;
    using JunctionPair = std::pair<JunctionIndex, JunctionIndex>;
    using JunctionPairs = std::vector<JunctionPair>;
    using Distance = long long;
    using DistanceMap = std::map<Distance, JunctionPairs>;

    template <std::size_t L>
    JunctionMap getJunctionMap(const Lines<L>& lines)
    {
        JunctionMap map {};
        JunctionIndex index {0};
        for (std::string_view line : lines)
        {
            if (line.find(',') == std::string_view::npos)
                continue;
            std::string row { std::string(line) };
            
            size_t firstComma { row.find(',') };
            size_t secondComma { row.find(',', firstComma + 1) };

            Junction junc {
                std::stoll(row.substr(0, firstComma)),
                std::stoll(row.substr(firstComma + 1, secondComma - (firstComma + 1))),
                std::stoll(row.substr(secondComma + 1))
            };

            map[index] = junc;
            ++index;
        }
        return map;
    }

    Distance getDistance(const Junction& junc1, const Junction& junc2)
    {
        Distance dist1 { std::get<0>(junc1) - std::get<0>(junc2) };
        Distance dist2 { std::get<1>(junc1) - std::get<1>(junc2) };
        Distance dist3 { std::get<2>(junc1) - std::get<2>(junc2) };
        return (dist1 * dist1) + (dist2 * dist2) + (dist3 * dist3);
    }

    DistanceMap makeDistanceMap(const JunctionMap& junctionMap)
    {
        DistanceMap distanceMap {};
        for (JunctionIndex junc1{0}; junc1 < junctionMap.size(); ++junc1)
        {
            for (JunctionIndex junc2{junc1 + 1}; junc2 < junctionMap.size(); ++junc2)
            {
                Distance dist { getDistance(junctionMap.at(junc1), junctionMap.at(junc2)) };
                distanceMap[dist].push_back(std::make_pair(junc1, junc2));
            }
        }
        return distanceMap;
    }

    template <std::size_t L>
    BoxInt getJunctionCoordinates(const Lines<L>& lines)
    {
        JunctionMap junctionMap { getJunctionMap<L>(lines) };
        DistanceMap distanceMap { makeDistanceMap(junctionMap) };

        Network network(junctionMap.size());
        long long connected {0};
        for (const auto& juncPairs : distanceMap | std::ranges::views::values)
        {
            for (const JunctionPair pair : juncPairs)
            {
                network.addEdge(pair.first, pair.second);
                network.addEdge(pair.second, pair.first);
                ++connected;

                if (connected >= junctionMap.size() - 1)
                {
                    long long vertexWithMaxConnect { network.getVertexWithMostConnections() };
                    long long mostConnections { network.findGraphSizeForANode(vertexWithMaxConnect) };

                    if (mostConnections == junctionMap.size())
                    {
                        return (std::get<0>(junctionMap.at(pair.first)) * std::get<0>(junctionMap.at(pair.second)));
                    }
                    else
                    {
                        connected -= (junctionMap.size() - mostConnections);
                    }
                }
            }
        }

        throw std::out_of_range("Didn't find a solution.\n");
    }
}

#endif // AOC8B_H

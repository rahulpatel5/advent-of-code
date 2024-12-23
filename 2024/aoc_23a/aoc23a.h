#ifndef AOC23A_H
#define AOC23A_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

/*
read all connection pairs
find all unique computers (combine with above for speed)
for all combinations of 3 computers, find which are connected
and which have at least one computer starting with a 't'

CORRECTION:
    didn't spot that computer needs to START with 't'
    need to correct for that
*/

namespace aoc23a
{
    using NetworkInt = long long;
    using Computer = std::string;
    using UniqueComputers = std::set<Computer>;
    using Computers = std::vector<Computer>;
    using Connection = std::pair<Computer, Computer>;
    using Connections = std::vector<Connection>;
    template <int N>
    using Network = std::array<Computer, N>;
    template <int N>
    using Networks = std::vector<Network<N>>;

    // this function has the side effect of collecting unique computers
    // a side effect of collecting valid computers (starting with 't')
    template <std::size_t N>
    Connections getConnections(const std::array<std::string_view, N>& lines, UniqueComputers& uniqueComputers, UniqueComputers& validComputers, char valid)
    {
        Connections connections {};
        for (std::string_view line : lines)
        {
            size_t hyphen {line.find('-')};
            std::string firstComputer {line.substr(0, hyphen)};
            std::string secondComputer {line.substr(hyphen + 1)};
            connections.push_back({firstComputer, secondComputer});
            uniqueComputers.insert(firstComputer);
            uniqueComputers.insert(secondComputer);
            if (firstComputer.at(0) == valid)
                validComputers.insert(firstComputer);
            if (secondComputer.at(0) == valid)
                validComputers.insert(secondComputer);
        }
        return connections;
    }

    template <int TvalidNetwork>
    Networks<TvalidNetwork> getCombinations(const Computers& unique)
    {
        Networks<TvalidNetwork> combinations {};
        // another instance of a bodge at generating combinations
        // area to improve with a better (generic) combination class/function
        for (size_t i{0}; i < unique.size(); ++i)
        {
            for (size_t j{i + 1}; j < unique.size(); ++j)
            {
                for (size_t k{j + 1}; k < unique.size(); ++k)
                    combinations.push_back({unique[i], unique[j], unique[k]});
            }
        }
        return combinations;
    }

    template <int TvalidNetwork>
    bool hasValidComputer(const Network<TvalidNetwork>& combination, const UniqueComputers& validComputers)
    {
        // can't use std::find for this. Consider if faster way than for loop
        for (const Computer& computer : combination)
        {
            if (validComputers.find(computer) != validComputers.end())
                return true;
        }
        return false;
    }

    template <int TvalidNetwork>
    bool isValidNetwork(const Network<TvalidNetwork>& combination, const Connections& connections)
    {
        // this uses that TvalidNetwork = 3, so is not flexible
        // testing whether this code format works as intended
        return (
            (
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(0), combination.at(1))) != connections.end())
                ||
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(1), combination.at(0))) != connections.end())
            )
            &&
            (
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(1), combination.at(2))) != connections.end())
                ||
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(2), combination.at(1))) != connections.end())
            )
            &&
            (
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(2), combination.at(0))) != connections.end())
                ||
                (std::find(connections.begin(), connections.end(), std::make_pair(combination.at(0), combination.at(2))) != connections.end())
            ));
    }

    template <int TvalidNetwork>
    NetworkInt countValidNetworks(const Networks<TvalidNetwork>& combinations, const UniqueComputers& validComputers, const Connections& connections)
    {
        NetworkInt count {0};
        for (const Network<TvalidNetwork>& combination : combinations)
        {
            if (hasValidComputer<TvalidNetwork>(combination, validComputers) && isValidNetwork<TvalidNetwork>(combination, connections))
                ++count;
        }
        return count;
    }


    template <std::size_t N, int TvalidNetwork>
    NetworkInt parseAndCountValidNetworks(const std::array<std::string_view, N>& lines, char valid)
    {
        UniqueComputers uniqueSet {};
        UniqueComputers validComputers {};
        Connections connections {getConnections<N>(lines, uniqueSet, validComputers, valid)};
        // it's easier to access elements of a vector
        // so this is a hacky way to work around to not use a set
        Computers unique {uniqueSet.begin(), uniqueSet.end()};
        Networks<TvalidNetwork> networkCombinations {getCombinations<TvalidNetwork>(unique)};
        return countValidNetworks<TvalidNetwork>(networkCombinations, validComputers, connections);
    }
}

#endif // AOC23A_H

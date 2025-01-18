#ifndef AOC23B_H
#define AOC23B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "NetworkGraph.h"

/*
read all connection pairs
find size of networks, i.e. how many computers are connected
(could try using a vector of sets, and getting the biggest. Would be most direct, but may take a long time)
for longest, sort in alphabetical order (set already does this) and generate a string separated by commas

try iterating each unique computer, and counting how big a chain can get
i.e. start with computer A, find link with computer B
computer C connects to computer B etc

need to be smart about selecting combinations otherwise it'll take ages

issue is finding the stopping point. Currently keeps looking
either need to get all loops using set (likely take ages)
or do something clever that I can't figure out to keep only the longest

we can collect the largest loop, i.e. track whenever we get a closed loop
then replace this collector with a bigger closed loop if we find one
we stop looking once we don't get any more unique computers to add
or once we have looped for the total number of computers there are
the wrinkle is we need to track each branch of connections
this makes it complex to set up and track
if that's done right, it should in theory be fine to solve

it's a waste of time to generate connections every time
generate and store them in a map, to access later

could a graph be a good approach?
seems promising
another thing that could help is setting the max iterations to links
assuming that all computers in the network must be connected
this reduces the total number of loops that are needed

we're iterating over every connection we can get
however, can we instead use the fact that all computers in a network join
we can look at adjacent lists and see which points are shared
can we use that to reduce the number of loops we have?

even better, since all computers need to be connected
can we look at which elements are shared between each current set
could also use memoisation to store shared connections between two points
*/

namespace aoc23b
{
    using Password = std::string;
    using Computer = std::string;
    using Computers = std::vector<Computer>;
    using UniqueComputers = std::set<Computer>;

    using Connection = std::pair<Computer, Computer>;
    using Connections = std::vector<Connection>;

    using IntComputer = int;
    using IntComputers = std::vector<IntComputer>;
    using StrToIntMap = std::map<std::string, IntComputer>;
    using IntToStrMap = std::map<IntComputer, std::string>;
    using IntConnection = std::pair<IntComputer, IntComputer>;
    using IntConnections = std::vector<IntConnection>;
    using UniqueIntComputers = std::set<IntComputer>;

    // this function has the side effect of collecting unique computers
    template <std::size_t N>
    Connections getConnections(const std::array<std::string_view, N>& lines, UniqueComputers& uniqueComputers)
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
        }
        return connections;
    }

    void computerConversion(StrToIntMap& stringToInt, IntToStrMap& intToString, const UniqueComputers& uniqueSet)
    {
        int index {0};
        for (const Computer& computer : uniqueSet)
        {
            stringToInt[computer] = index;
            intToString[index] = computer;
            ++index;
        }
    }

    IntConnections getIntConnections(const Connections& connections, const StrToIntMap& strToInt)
    {
        IntConnections intConnections {};
        for (const Connection& conn : connections)
        {
            intConnections.push_back({strToInt.at(conn.first), strToInt.at(conn.second)});
        }
        return intConnections;
    }

    UniqueIntComputers findLargestNetwork(const NetworkGraph& graph)
    {
        return graph.getLongestLoop();
    }

    Password getPassword(const UniqueIntComputers& networkInt, const IntToStrMap& intToStr)
    {
        // first change to string
        UniqueComputers networkStr {};
        for (IntComputer computerInt : networkInt)
            networkStr.insert(intToStr.at(computerInt));
        
        // now get password
        Password password {};
        bool isFirstLoop {true};
        for (const Computer& computer : networkStr)
        {
            if (!isFirstLoop)
                password.append(",");
            password.append(computer);
            isFirstLoop = false;
        }
        return password;
    }

    template <std::size_t N>
    Password parseAndGetPassword(const std::array<std::string_view, N>& lines)
    {
        UniqueComputers uniqueSet {};
        Connections connectionsStr {getConnections<N>(lines, uniqueSet)};
        // it'll be easier to use a graph with int
        // so convert computers from string to int
        StrToIntMap stringToInt {};
        IntToStrMap intToString {};
        computerConversion(stringToInt, intToString, uniqueSet);
        IntConnections connectionsInt {getIntConnections(connectionsStr, stringToInt)};

        NetworkGraph graph(uniqueSet.size());
        // add edges
        for (const IntConnection& i : connectionsInt)
        {
            graph.addEdge(i.first, i.second);
            graph.addEdge(i.second, i.first);
        }

        UniqueIntComputers largestNetwork {findLargestNetwork(graph)};
        return getPassword(largestNetwork, intToString);
    }
}

#endif // AOC23B_H

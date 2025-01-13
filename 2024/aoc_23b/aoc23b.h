#ifndef AOC23B_H
#define AOC23B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <map>

#include <iostream>

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
*/

namespace aoc23b
{
    using Password = std::string;
    using Computer = std::string;
    using Computers = std::vector<Computer>;
    using ComputersList = std::vector<Computers>;
    using UniqueComputers = std::set<Computer>;
    using ConnectDict = std::map<Computer, Computers>;

    using Connection = std::pair<Computer, Computer>;
    using Connections = std::vector<Connection>;
    using Networks = std::vector<Connections>;
    using UniqueConnections = std::set<Connection>;

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

    Computers getComputerConnections(const Computer& computer, const Connections& connections)
    {
        Computers joined {};
        for (const Connection& connection : connections)
        {
            if (connection.first == computer)
                joined.push_back(connection.second);
            else if (connection.second == computer)
                joined.push_back(connection.first);
        }
        return joined;
    }

    size_t findLongestNetwork(const ComputersList& networks)
    {
        size_t longestIndex {};
        size_t largestSize {};
        for (size_t i{0}; i < networks.size(); ++i)
        {
            if (networks.at(i).size() > largestSize)
            {
                largestSize = networks.at(i).size();
                longestIndex = i;
            }
        }
        return longestIndex;
    }

    // function has side effect of adding to ConnectDict
    UniqueComputers connectOtherComputers(const Computer& computer, ConnectDict& connectDict, const Connections& connections, size_t maxIterations)
    {
        Computers network {};
        network.push_back(computer);
        ComputersList allNetworks {};
        allNetworks.push_back(network);
        ComputersList loopedNetworks {};

        // some variables to handle the do-while loop
        size_t loops {0};
        int sizeCounter {};
        do
        {
            std::cout << "\tloop size: " << allNetworks.size() << '\n';
            sizeCounter = 0;
            ComputersList newAllNetworks {};
            for (const Computers& network : allNetworks)
            {
                Computer current {network.back()};
                // std::cout << current << ": ";
                if (connectDict.find(current) == connectDict.end())
                    connectDict[current] = getComputerConnections(current, connections);
                
                Computers currentNetwork {network};
                for (const Computer& pairedComp : connectDict.at(current))
                {
                    // std::cout << pairedComp << ' ';
                    if (std::find(currentNetwork.begin(), currentNetwork.end(), pairedComp) != currentNetwork.end())
                    {
                        if (pairedComp == computer)
                            loopedNetworks.push_back(currentNetwork);
                        continue;
                    }
                    Computers tempNetwork {currentNetwork};
                    tempNetwork.push_back(pairedComp);
                    newAllNetworks.push_back(tempNetwork);
                    ++sizeCounter;
                }
                // std::cout << '\n';
            }
            allNetworks = newAllNetworks;
        } while (sizeCounter > 0 && loops < maxIterations);
        
        // std::cout << "total looped: " << loopedNetworks.size() << '\n';
        // find the longest network
        size_t longestIndex {findLongestNetwork(loopedNetworks)};
        UniqueComputers largestNetwork {loopedNetworks[longestIndex].begin(), loopedNetworks[longestIndex].end()};
        // std::cout << "largest: " << loopedNetworks[longestIndex].size() << '\n';
        // for (auto c : loopedNetworks[longestIndex])
        //     std::cout << c << ' ';
        // std::cout << '\n';
        return largestNetwork;
    }

    UniqueComputers getNetworks(const Computers& unique, const Connections& connections)
    {
        // collect connections to save time
        ConnectDict connectDict {};
        UniqueComputers largestNetwork {};
        // when iterating through networks, max is total number of computers
        // see if there's a way to update this to reduce time?
        size_t maxIterations {unique.size()};

        // logic is we go through each unique computer and find networks
        // but if the largest network so far is 15 out of 30 total computers
        // it's a waste of time searching the back 15 computers
        // so we shorten the for loop by the size of the largest network
        // first condition (with arbitrary number) to ensure first loops run
        std::cout << "unique: " << unique.size() << '\n';
        for (size_t i{0}; i < unique.size() - largestNetwork.size(); ++i)
        {
            // std::cout << "iter: " << i << " of " << unique.size() - largestNetwork.size() - 1 << '\n';
            // if (connectDict.find(unique[i]) == connectDict.end())
            //     connectDict[unique[i]] = getComputerConnections(unique[i], connections);
            // UniqueComputers innerNetwork {connectOtherComputers(unique[i], connectDict, connections, maxIterations)};
            // if (innerNetwork.size() > largestNetwork.size())
            //     largestNetwork = innerNetwork;
            // std::cout << i << " fin " << largestNetwork.size() << '\n';
            connectDict[unique[i]] = getComputerConnections(unique[i], connections);
        }
        // for (auto [key, value] : connectDict)
        //     std::cout << key << ' ' << value.size() << '\n';
        return largestNetwork;
    }

    Password getPassword(const UniqueComputers& network)
    {
        Password password {};
        bool isFirstLoop {true};
        for (const Computer& computer : network)
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
        Connections connections {getConnections<N>(lines, uniqueSet)};
        // it's easier to access elements of a vector
        // so this is a hacky way to work around to not use a set
        Computers unique {uniqueSet.begin(), uniqueSet.end()};
        UniqueComputers largestNetwork {getNetworks(unique, connections)};
        return getPassword(largestNetwork);
    }
}

#endif // AOC23B_H

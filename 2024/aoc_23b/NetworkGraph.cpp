#include "NetworkGraph.h"
#include <set>
#include <map>

std::set<int> getSharedConnections(const std::set<int>& network, const std::map<int, std::set<int>>& adjacent, std::map<std::set<int>, std::set<int>>& shared)
{
    // if we've already run this network, add to the shared collection
    if (shared.find(network) != shared.end())
        return shared.at(network);
    // if there's only one element in network, we can use adjacent
    if (network.size() == 1)
        return adjacent.at(*network.begin());

    std::set<int> linked {};
    for (int c : network)
    {
        if (linked.size() == 0)
            linked = adjacent.at(c);
        else
        {
            for (auto it {linked.begin()}; it != linked.end(); )
            {
                if (adjacent.at(c).find(*it) == adjacent.at(c).end())
                    it = linked.erase(it);
                else
                    ++it;
                // also handle if linked has become empty
                if (linked.size() == 0)
                    break;
            }
            // need to handle if we have no shared elements left
            if (linked.size() == 0)
                break;
        }
    }
    shared[network] = linked;
    return linked;
}

std::set<int> findLongLoop(int start, int maxIterations, const std::map<int, std::set<int>>& adjacent, std::map<std::set<int>, std::set<int>>& shared)
{
    std::set<int> longLoop {};

    std::set<std::set<int>> current {};
    current.insert({start});
    for (int i{1}; i <= maxIterations; ++i)
    {
        if (current.size() == 0)
            break;
        std::set<std::set<int>> next {};
        for (const std::set<int>& list : current)
        {
            // this happens to avoid duplicate computers
            std::set<int> joiners {getSharedConnections(list, adjacent, shared)};
            // if no more computers to add, we're at max size
            if (joiners.size() == 0 && list.size() > longLoop.size())
                longLoop = list;

            for (int join : joiners)
            {
                std::set<int> newNetwork {list};
                newNetwork.insert(join);
                next.insert(newNetwork);
            }
        }
        current = std::move(next);
    }
    
    return longLoop;
}

std::set<int> NetworkGraph::getLongestLoop() const
{
    std::set<int> longest {};
    // since all computers must be connected in the network
    // the max is the number of connections for one computer
    // add 1 as we may need this to make sure we get full loops
    int maxIter {static_cast<int>(m_adjacent.at(0).size() + 1)};

    // collect shared connections between two computers (memoisation)
    std::map<std::set<int>, std::set<int>> shared {};

    for (int v{0}; v < static_cast<int>(m_vertices - (longest.size() - 1)); ++v)
    {
        std::set<int> loop {findLongLoop(v, maxIter, m_adjacent, shared)};
        if (loop.size() > longest.size())
            longest = loop;
    }

    return longest;
}

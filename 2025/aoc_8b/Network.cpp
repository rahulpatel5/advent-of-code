#include <map>
#include <set>
#include <vector>
#include "Network.h"

long long Network::findGraphSizeForANode(long long a) const
{
    std::set<long long> graph { a };
    std::vector<long long> currentNodes { a };
    while (currentNodes.size() > 0)
    {
        std::vector<long long> nextNodes {};
        for (long long checkNode : currentNodes)
        {
            for (long long nextNode : m_adjacent.at(checkNode))
            {
                if (graph.find(nextNode) != graph.end())
                    continue;
                graph.insert(nextNode);
                nextNodes.push_back(nextNode);
            }
        }
        currentNodes = nextNodes;
    }
    return graph.size();
}

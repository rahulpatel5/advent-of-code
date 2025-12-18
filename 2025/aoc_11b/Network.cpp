#include <map>
#include <set>
#include "Network.h"

long long Network::findPathsBetween(long long start, long long end, long long falseEnd1, long long falseEnd2)
{
    long long validPaths {0};
    std::map<long long, std::pair<std::set<long long>, long long>> paths {};
    std::set<long long> empty {};
    paths[end] = std::make_pair(empty, 1);
    long long loops {0};
    while (paths.size() > 0 && loops < m_vertices)
    {
        std::map<long long, std::pair<std::set<long long>, long long>> nextPaths {};
        for (const auto& [node, values] : paths)
        {
            for (long long nextNode : m_adjacent.at(node))
            {
                if (nextNode == end || nextNode == falseEnd1 || nextNode == falseEnd2 || values.first.find(nextNode) != values.first.end())
                    continue;
                else if (nextNode == start)
                {
                    validPaths += values.second;
                    continue;
                }
                else
                {
                    if (nextPaths.find(nextNode) == nextPaths.end())
                    {
                        std::set<long long> newUnique { values.first };
                        newUnique.insert(nextNode);
                        nextPaths[nextNode] = std::make_pair(newUnique, values.second);
                    }
                    else
                    {
                        // nextNode should already be in existing
                        std::set<long long> updateUnique { nextPaths.at(nextNode).first };
                        updateUnique.insert(values.first.begin(), values.first.end());
                        long long updateMultiplier { nextPaths.at(nextNode).second };
                        updateMultiplier += values.second;
                        nextPaths[nextNode] = std::make_pair(updateUnique, updateMultiplier);
                    }
                }
            }
        }
        paths = nextPaths;
        ++loops;
    }
    return validPaths;
}

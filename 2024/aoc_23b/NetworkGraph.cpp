#include "NetworkGraph.h"
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include <map>

#include <iostream>
#include "../../shared/Timer.h"

int factorial(int num, std::map<int, int>& factorials)
{
    if (factorials.find(num) != factorials.end())
        return factorials.at(num);
    // we set the base case before, so just apply the rest
    factorials[num] = num * factorial(num - 1, factorials);
    return factorials[num];
}

int combinations(int num, std::map<int, int>& factorials)
{
    if (num == 2)
        return 1;
    // nCr = n! / (r! (n-r)!)
    // hard-coding that there are 2 elements chosen per combination
    return factorial(num, factorials) / factorial(2, factorials) / factorial(num - 2, factorials);
}

bool isEnoughEdges(const std::list<int>& list, const std::list<int>* adjacent, std::map<int, int>& factorials)
{
    int target {combinations(static_cast<int>(list.size()), factorials)};
    int edges {0};
    // std::cout << "for " << list.size() << ' ' << target << '\n';
    for (auto indexOuter {list.begin()}; indexOuter != list.end(); ++indexOuter)
    {
        for (auto indexInner {std::next(indexOuter)}; indexInner != list.end(); ++indexInner)
        {
            if (std::find(adjacent[*indexOuter].begin(), adjacent[*indexOuter].end(), *indexInner) != adjacent[*indexOuter].end())
                ++edges;
        }
        // if (count < target)
        //     return false;
    }
    return edges >= target;
}

std::list<int> findLongLoop(int start, int maxIterations, const std::list<int>* adjacent, std::map<int, int>& factorials)
{
    std::list<int> longLoop {};

    std::set<std::list<int>> current {};
    current.insert({start});
    for (int i{1}; i <= maxIterations; ++i)
    {
        // std::cout << '\t' << i << ' ' << current.size() << ' ' << longLoop.size() << '\n';
        std::set<std::list<int>> next {};
        for (std::list<int> list : current)
        {
            for (int connection : adjacent[list.back()])
            {
                if (connection == start)
                {
                    if (list.size() > longLoop.size())
                        longLoop = list;
                }
                // check if new vertex has an edge with the start
                // if not, we can skip to save time
                else if (std::find(adjacent[start].begin(), adjacent[start].end(), connection) == adjacent[start].end())
                    continue;
                // add linked vertex, if not already visited
                else if (std::find(list.begin(), list.end(), connection) == list.end())
                {
                    std::list<int> temp {list};
                    temp.push_back(connection);
                    if (isEnoughEdges(temp, adjacent, factorials))
                        next.insert(temp);
                }
            }
        }
        current = next;
    }
    
    return longLoop;
}

std::list<int> NetworkGraph::getLongestLoop() const
{
    std::list<int> longest {};
    int maxIter {static_cast<int>(m_adjacent[0].size())};
    std::map<int, int> factorials {};
    // set base case
    factorials[1] = 1;
    // std::cout << "max: " << maxIter << '\n';

    for (int v{0}; v < maxIter; ++v)
    {
        // std::cout << "loop " << v << '\n';
        std::list<int> loop {findLongLoop(v, maxIter, m_adjacent, factorials)};
        // std::cout << v << ": " << loop.size() << '\n';
        if (loop.size() > longest.size())
            longest = loop;
        // std::cout << v << ": ";
        // for (int n : longest)
        //     std::cout << n << ' ';
        // std::cout << '\n';
    }

    return longest;
}

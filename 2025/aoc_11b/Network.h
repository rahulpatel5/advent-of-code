#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <set>

/*
re-uses NetworkGraph.h in 2024 23 part 2 solution:
https://github.com/rahulpatel5/advent-of-code/blob/main/2024/aoc_23b/NetworkGraph.h
*/

class Network
{
private:
    long long m_vertices;
    std::map<long long, std::set<long long>> m_adjacent;

public:
    Network(long long vertices)
        : m_vertices {vertices}
        , m_adjacent {}
    {}

    void addEdge(long long a, long long b) { m_adjacent[a].insert(b); }

    long long findPathsBetween(long long start, long long end, long long falseEnd1, long long falseEnd2);
};

#endif // NETWORK_H

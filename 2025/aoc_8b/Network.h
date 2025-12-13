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
    long long m_max_adjacent;
    long long m_vertex_with_max_adjacent;

public:
    Network(long long vertices)
        : m_vertices {vertices}
        , m_adjacent {}
        , m_max_adjacent {}
        , m_vertex_with_max_adjacent {}
    {}

    void addEdge(long long a, long long b) {
        m_adjacent[a].insert(b);
        if (m_adjacent.at(a).size() > m_max_adjacent)
        {
            m_max_adjacent = m_adjacent.at(a).size();
            m_vertex_with_max_adjacent = a;
        }
    }

    long long getVertexWithMostConnections() const { return m_vertex_with_max_adjacent; }

    bool hasVertex(long long a) const { return m_adjacent.find(a) != m_adjacent.end(); }

    long long getNumberOfConnections(long long a) const { return m_adjacent.at(a).size(); }

    long long findGraphSizeForANode(long long a) const;
};

#endif // NETWORK_H

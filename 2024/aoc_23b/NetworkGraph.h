#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <map>
#include <set>

/*
adapts code from:
https://medium.com/@muskansanghai/understanding-graphs-in-c-a-beginners-guide-cbbba24b390e
*/

class NetworkGraph
{
private:
    int m_vertices;
    std::map<int, std::set<int>> m_adjacent;

public:
    NetworkGraph(int vertices)
        : m_vertices {vertices}
        , m_adjacent {}
    {}

    void addEdge(int a, int b) { m_adjacent[a].insert(b); }

    std::set<int> getLongestLoop() const;
};

#endif // NETWORKGRAPH_H

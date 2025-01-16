#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <list>

#include <iostream>

/*
adapts code from:
https://medium.com/@muskansanghai/understanding-graphs-in-c-a-beginners-guide-cbbba24b390e
*/

class NetworkGraph
{
private:
    int m_vertices;
    std::list<int>* m_adjacent;

public:
    NetworkGraph(int vertices)
        : m_vertices {vertices}
        , m_adjacent {new std::list<int>[vertices]}
    {}

    void addEdge(int a, int b) { m_adjacent[a].push_back(b); }

    std::list<int> getLongestLoop() const;

    // void printGraph()
    // {
    //     for (int v{0}; v < m_vertices; ++v)
    //     {
    //         std::cout << v << ": ";
    //         for (int e : m_adjacent[v])
    //             std::cout << e << ' ';
    //         std::cout << '\n';
    //     }
    // }
};

#endif // NETWORKGRAPH_H

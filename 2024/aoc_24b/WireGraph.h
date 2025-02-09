#ifndef WIREGRAPH_H
#define WIREGRAPH_H

#include <map>
#include <set>
#include <vector>
#include <cstdint>
#include <ranges>

/*
think we want a directed oriented graph
i.e. there's direction between vertices (input to output)
also want to keep info about value of wire (vertex) - using vertex data?
or does the vertex data need to be the wire label?
if we have directed, does that mean each vertex has a head and tail?
do we want/need to, and can we, keep info about AND/OR/XOR?
can weights hold some of the information?

deep copy function uses:
https://www.learncpp.com/cpp-tutorial/shallow-vs-deep-copying/
*/

class WireGraph
{
private:
    struct Node
    {
        std::int16_t m_wire;
        std::int16_t m_value; // 0 or 1
        std::int16_t m_operation; // 1 for AND, 2 for OR, 3 for XOR, 0 otherwise
        std::vector<Node*> m_inputs;
        std::vector<Node*> m_outputs;

        Node(std::int16_t wire)
            : m_wire {wire}
            , m_value {-1} // default value so we know when value was set
        {}
    };
    std::int16_t m_vertices;
    std::vector<Node*> m_wires;

public:
    WireGraph(std::int16_t vertices);

    void deepcopy(const WireGraph& source);
    // copy constructor
    WireGraph(const WireGraph& source) { deepcopy(source); }
    // assignment operator
    WireGraph& operator=(const WireGraph& source);
    // destructor
    ~WireGraph()
    {
        for (auto node : m_wires)
            delete node;
    }

    void addEdge(std::int16_t a, std::int16_t b);
    void removeEdge(std::int16_t a, std::int16_t b);

    void setValue(std::int16_t a, std::int16_t b) { m_wires[a]->m_value = b; }
    std::int16_t getValue(std::int16_t a) const { return m_wires[a]->m_value; }
    void setOper(std::int16_t a, std::int16_t oper) { m_wires[a]->m_operation = oper; }
    std::int16_t getOper(std::int16_t a) const { return m_wires[a]->m_operation; }
    std::int16_t getTotalVertex() const { return m_vertices; }

    std::pair<std::int16_t, std::int16_t> getInputs(std::int16_t a);
    bool hasNoInput(std::int16_t a) const { return m_wires[a]->m_inputs.empty(); }
    std::vector<std::int16_t> getOutputs(std::int16_t a);

    void swapNodes(std::int16_t a, std::int16_t b);
};

#endif // WIREGRAPH_H

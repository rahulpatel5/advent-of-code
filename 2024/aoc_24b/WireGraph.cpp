#include "WireGraph.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <algorithm>

WireGraph::WireGraph(std::int16_t vertices)
    : m_vertices {vertices}
{
    for (std::int16_t i{0}; i < vertices; ++i)
        m_wires.push_back(new Node(i));
}

void WireGraph::deepcopy(const WireGraph& source)
{
    m_vertices = source.m_vertices;
    m_wires.clear();
    m_wires.reserve(m_vertices);

    std::unordered_map<Node*, Node*> nodeMapping {};
    for (const auto& node : source.m_wires)
    {
        Node* new_node {new Node(node->m_wire)};
        new_node->m_value = node->m_value;
        new_node->m_operation = node->m_operation;
        m_wires.push_back(new_node);
        nodeMapping[node] = new_node;
    }
    
    for (size_t i{0}; i < m_wires.size(); ++i)
    {
        for (const auto& input : source.m_wires[i]->m_inputs)
            m_wires[i]->m_inputs.push_back(nodeMapping[input]);
        
        for (const auto& output : source.m_wires[i]->m_outputs)
            m_wires[i]->m_outputs.push_back(nodeMapping[output]);
    }
}

WireGraph& WireGraph::operator=(const WireGraph& source)
{
    if (this != &source)
        deepcopy(source);
    return *this;
}

// a = output wire, b = input wire
void WireGraph::addEdge(std::int16_t a, std::int16_t b)
{
    m_wires[a]->m_inputs.push_back(m_wires[b]);
    m_wires[b]->m_outputs.push_back(m_wires[a]);
}

// a = output wire, b = input wire
void WireGraph::removeEdge(std::int16_t a, std::int16_t b)
{
    auto& inputs {m_wires[a]->m_inputs};
    inputs.erase(std::remove(inputs.begin(), inputs.end(), m_wires[b]), inputs.end());

    auto& outputs {m_wires[b]->m_outputs};
    outputs.erase(std::remove(outputs.begin(), outputs.end(), m_wires[a]), outputs.end());
}

std::pair<std::int16_t, std::int16_t> WireGraph::getInputs(std::int16_t a)
{
    assert(m_wires[a]->m_inputs.size() == 2 && "There are not two inputs.\n");
    std::pair<std::int16_t, std::int16_t> pair {};
    pair.first = m_wires[a]->m_inputs.at(0)->m_wire;
    pair.second = m_wires[a]->m_inputs.at(1)->m_wire;
    return pair;
}

std::vector<std::int16_t> WireGraph::getOutputs(std::int16_t a)
{
    std::vector<std::int16_t> outputs {};
    for (Node* node : m_wires[a]->m_outputs)
        outputs.push_back(node->m_wire);
    return outputs;
}

void WireGraph::swapNodes(std::int16_t a, std::int16_t b)
{
    // assume we do not have nodes that are inputs only
    // collect inputs for each node
    std::vector<std::int16_t> aInputs {};
    for (Node* input : m_wires[a]->m_inputs)
        aInputs.push_back(input->m_wire);
    std::vector<std::int16_t> bInputs {};
    for (Node* input : m_wires[b]->m_inputs)
        bInputs.push_back(input->m_wire);
    // if aInputs and bInputs are the same, we should skip to avoid issues
    std::sort(aInputs.begin(), aInputs.end());
    std::sort(bInputs.begin(), bInputs.end());
    if (aInputs != bInputs)
    {
        // remove and add the edges for each node
        for (std::int16_t input : aInputs)
        {
            removeEdge(a, input);
            addEdge(b, input);
        }
        for (std::int16_t input : bInputs)
        {
            removeEdge(b, input);
            addEdge(a, input);
        }
    }
    // swap the operators and values
    std::swap(m_wires[a]->m_operation, m_wires[b]->m_operation);
    std::swap(m_wires[a]->m_value, m_wires[b]->m_value);
}

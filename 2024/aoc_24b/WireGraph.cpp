#include "WireGraph.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>

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

void WireGraph::operate(std::int16_t a)
{
    // if only input, 
    if (m_wires[a]->m_inputs.empty())
        {/* intentionally empty */}
    // 1 for AND
    else if (m_wires[a]->m_operation == 1)
        m_wires[a]->m_value = m_wires[a]->m_inputs.at(0)->m_value & m_wires[a]->m_inputs.at(1)->m_value;
    // 2 for OR
    else if (m_wires[a]->m_operation == 2)
        m_wires[a]->m_value = m_wires[a]->m_inputs.at(0)->m_value | m_wires[a]->m_inputs.at(1)->m_value;
    // 3 for XOR
    else if (m_wires[a]->m_operation == 3)
        m_wires[a]->m_value = m_wires[a]->m_inputs.at(0)->m_value ^ m_wires[a]->m_inputs.at(1)->m_value;
}

void WireGraph::swapNodes(std::int16_t a, std::int16_t b)
{
    // if we have nodes that are inputs only, handle differently
    if (hasNoInput(a) && hasNoInput(b))
    {
        // don't know what should happen here
        // try swapping values only
        std::swap(m_wires[a]->m_value, m_wires[b]->m_value);
    }
    else if (!hasNoInput(a) && !hasNoInput(b))
    {
        // collect inputs for each node
        std::vector<std::int16_t> aInputs {};
        for (Node* input : m_wires[a]->m_inputs)
            aInputs.push_back(input->m_wire);
        std::vector<std::int16_t> bInputs {};
        for (Node* input : m_wires[b]->m_inputs)
            bInputs.push_back(input->m_wire);
        // remove and add those edges for each node
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
        // swap the operators and values
        std::swap(m_wires[a]->m_operation, m_wires[b]->m_operation);
        std::swap(m_wires[a]->m_value, m_wires[b]->m_value);
    }
    else
        throw std::invalid_argument("Given a mix of a wire with no input and a wire with inputs to swap.\n");
}

void WireGraph::updateOutputs(std::int16_t a)
{
    // iterate through outputs and update values
    // we'll do this by collecting new outputs in each loop
    std::vector<Node*> current {m_wires[a]->m_outputs};
    while (!current.empty())
    {
        std::vector<Node*> next {};
        for (Node* node : current)
        {
            operate(node->m_wire);
            next.insert(next.end(), node->m_outputs.begin(), node->m_outputs.end());
        }
        current = next;
    }
}

std::vector<std::int16_t> WireGraph::getAllInputs(std::int16_t a) const
{
    std::vector<std::int16_t> inputs {};
    std::vector<Node*> current {};
    current.push_back(m_wires[a]);
    // we're collecting too many inputs, so let's limit to 3 layers
    int layers {0};
    while (!current.empty() && layers < 3)
    {
        std::vector<Node*> next {};
        for (Node* node : current)
        {
            inputs.push_back(node->m_wire);
            next.insert(next.end(), node->m_inputs.begin(), node->m_inputs.end());
        }
        current = next;
        ++layers;
    }
    return inputs;
}

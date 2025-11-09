#ifndef AOC8A_H
#define AOC8A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

/*
READ network into a map

LOOP instructions:
    SELECT left/right choice
    INCREMENT step counter
    IF reach "ZZZ":
        BREAK

RETURN number of steps
*/

namespace aoc8a
{
    using StepInt = long long;
    template <std::size_t L>
    using Lines = std::array<std::string_view, L>;

    using Direction = char;
    using Instructions = std::string;

    using Node = std::string;
    using Nodes = std::tuple<Node, Node>;
    using Network = std::map<Node, Nodes>;
    using Index = size_t;

    template <std::size_t N>
    Instructions getInstructions(const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.find_first_not_of("LR") != std::string_view::npos && (line.find('L') != std::string_view::npos || line.find('R') != std::string_view::npos))
                continue;
            
            return std::string(line);
        }
        throw std::out_of_range("Did not find the instructions.\n");
    }

    template <std::size_t N>
    Network readNetwork(const Lines<N>& lines)
    {
        Network network {};

        for (std::string_view line : lines)
        {
            if (line.find('=') == std::string_view::npos)
                continue;

            std::string row { std::string(line) };

            size_t firstSpace { row.find(' ') };
            size_t startBracket { row.find('(') };
            size_t comma { row.find(',') };
            size_t lastSpace { row.find(' ', comma) };
            size_t closeBracket { row.find(')') };

            Node startNode { row.substr(0, firstSpace) };
            Node leftNode { row.substr(startBracket + 1, comma - (startBracket + 1)) };
            Node rightNode { row.substr(lastSpace + 1, closeBracket - (lastSpace + 1)) };

            network[startNode] = std::make_tuple(leftNode, rightNode);
        }

        return network;
    }

    template <std::size_t N>
    StepInt getSteps(const Lines<N>& lines)
    {
        Instructions instructions { getInstructions<N>(lines) };
        Network network { readNetwork<N>(lines) };

        StepInt steps {0};

        Index index {0};
        Node currentNode {"AAA"};
        while (true)
        {
            Direction direction {instructions.at(index)};
            Node nextNode {};
            if (direction == 'L')
            {
                nextNode = std::get<0>(network.at(currentNode));
            }
            else if (direction == 'R')
            {
                nextNode = std::get<1>(network.at(currentNode));
            }
            else
                throw std::out_of_range("Got a non-valid direction.\n");

            steps++;
            if (nextNode == "ZZZ")
                break;
            else
                currentNode = nextNode;

            index++;
            if (index >= instructions.size())
                index = 0;
            
            if (steps > 100'000'000)
            {
                throw std::out_of_range("Stopping script after taking too many steps.\n");
            }
        }

        return steps;
    }
}

#endif // AOC8A_H

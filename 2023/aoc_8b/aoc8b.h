#ifndef AOC8B_H
#define AOC8B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <ranges>
#include <limits>
#include <algorithm>
#include <numeric>
#include <cassert>

/*
READ network into a map
FIND starting points (end with 'A')

FOR each starting point:
    LOOP instructions:
        SELECT left/right choice
        INCREMENT step counter
    IF all new elements end with 'Z':
        BREAK

RETURN number of steps

takes too long for direct approach
try collecting when each path reaches a 'Z' node
may be able to find a pattern and get the solution that way?

may have to make an assumption about pattern (and check assumption)
*/

namespace aoc8b
{
    using StepInt = long long;
    template <std::size_t L>
    using Lines = std::array<std::string_view, L>;

    using Direction = char;
    using Instructions = std::string;

    using Node = std::string;
    using Nodes = std::tuple<Node, Node>;
    using NodePoints = std::vector<Node>;
    using Network = std::map<Node, Nodes>;
    using Index = size_t;

    using Steps = std::vector<StepInt>;
    using PatternMap = std::map<Index, Steps>;

    template <std::size_t N>
    Instructions getInstructions(const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.find_first_not_of("LR") != std::string_view::npos || line == "\n")
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

    NodePoints getStartNodes(const Network& network)
    {
        NodePoints startNodes {};
        for (const Node& node : network | std::views::keys)
        {
            if (node.back() == 'A')
                startNodes.push_back(node);
        }
        return startNodes;
    }

    bool nodeEndsZ(const Node& node)
    {
        return node.back() == 'Z';
    }

    bool allNodesEndZ(const NodePoints& nodes)
    {
        for (const Node& node : nodes)
        {
            if (!nodeEndsZ(node))
                return false;
        }
        return true;
    }

    StepInt getFewestMatches(const PatternMap& patternMap)
    {
        StepInt min {0};
        for (const Steps& s : patternMap | std::views::values)
        {
            if (min == 0 | static_cast<StepInt>(s.size()) < min)
                min = static_cast<StepInt>(s.size());
        }
        return min;
    }

    StepInt findShortestSteps(const Steps& startPoints, const Steps& repeatPatterns)
    {
        // check assumption
        for (size_t i{0}; i < startPoints.size(); ++i)
        {
            assert(startPoints.at(i) == repeatPatterns.at(i) && "Assumed that startPoints and repeatPatterns are the same. But this isn't the case.\n");
        }

        StepInt steps {};
        for (size_t j{0}; j < startPoints.size(); ++j)
        {
            if (j == 0)
            {
                steps = startPoints.at(j);
                continue;
            }
            steps = std::lcm(steps, startPoints.at(j));
        }
        return steps;
    }

    StepInt skipToAnswer(const PatternMap& patternMap)
    {
        Steps startingPoints {};
        Steps repeatingPatterns {};
        Index loopCount {1};
        for (const Steps& s : patternMap | std::views::values)
        {
            StepInt start { s.front() };
            StepInt gap {};
            for (size_t i{1}; i < s.size() / 2; ++i)
            {
                gap = s.at(i) - start;
                for (size_t j{1}; i + j < s.size(); ++j)
                {
                    if (s.at(i + j) - s.at(j) != gap)
                    {
                        gap = 0;
                        break;
                    }
                }
                if (gap != 0)
                {
                    startingPoints.push_back(start);
                    repeatingPatterns.push_back(gap);
                    break;
                }
            }

            if (repeatingPatterns.size() < loopCount)
                return -1;
            
            ++loopCount;
        }

        return findShortestSteps(startingPoints, repeatingPatterns);
    }

    template <std::size_t N>
    StepInt getSteps(const Lines<N>& lines)
    {
        Instructions instructions { getInstructions<N>(lines) };
        Network network { readNetwork<N>(lines) };

        StepInt steps {0};

        Index index {0};
        NodePoints currentNodes { getStartNodes(network) };
        PatternMap patternMap {};
        // only try patterns after there's a decent number of matches
        StepInt minMatches {5};
        StepInt loops {0};
        while (true)
        {
            NodePoints nextNodes {};
            Direction direction {instructions.at(index)};
            for (size_t i{0}; i < currentNodes.size(); ++i)
            {
                Node nextNode {};
                if (direction == 'L')
                {
                    nextNode = std::get<0>(network.at(currentNodes.at(i)));
                }
                else if (direction == 'R')
                {
                    nextNode = std::get<1>(network.at(currentNodes.at(i)));
                }
                else
                    throw std::out_of_range("Got a non-valid direction.\n");
                
                nextNodes.push_back(nextNode);

                if (nodeEndsZ(nextNode))
                {
                    patternMap[i].push_back(steps + 1);
                }
            }

            steps++;
            if (allNodesEndZ(nextNodes))
                break;
            else
                currentNodes = nextNodes;
            loops++;

            index++;
            if (index >= instructions.size())
                index = 0;
            
            StepInt newMinMatches { getFewestMatches(patternMap) };
            if (newMinMatches > minMatches)
            {
                minMatches = newMinMatches;
                StepInt answer { skipToAnswer(patternMap) };
                if (answer != -1)
                    return answer;
            }

            if (loops > 100'000'000)
            {
                throw std::out_of_range("Stopping script after taking too many loops.\n");
            }
        }

        return steps;
    }
}

#endif // AOC8B_H

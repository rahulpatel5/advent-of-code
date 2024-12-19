#ifndef AOC14B_H
#define AOC14B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>

/*
use part 1 solution but change to iterating each robot position each second
need to figure out way to identify when robots are in the required positions
will the topmost robot be central (i.e. in middle column)?
will all other robots be in a tree position?
will they form an outline of the tree, or fill too?
could the tree be offset?
could the tree be smaller than the full width of the map?

puzzle says that "most" of the robots arrange into a picture of a tree
we have the position of the robots, can we check some are in a diagonal?
*/

namespace aoc14b
{
    using Robot = std::pair<std::pair<int, int>, std::pair<int, int>>;
    using Robots = std::vector<Robot>;
    using VectorPairs = std::vector<std::pair<int, int>>;

    template <std::size_t N>
    Robots getStartingPositionsAndVelocities(const std::array<std::string_view, N>& lines)
    {
        VectorPairs positions {};
        VectorPairs velocities {};
        for (std::string_view line : lines)
        {
            size_t firstEquals {line.find('=')};
            size_t firstComma {line.find(',')};
            size_t space {line.find(' ')};
            int xPos {std::stoi(std::string(line.substr(firstEquals + 1, firstComma - (firstEquals + 1))))};
            int yPos {std::stoi(std::string(line.substr(firstComma + 1, space - (firstComma + 1))))};

            size_t secondEquals {line.find('=', firstEquals + 1)};
            size_t secondComma {line.find(',', firstComma + 1)};
            int xVel {std::stoi(std::string(line.substr(secondEquals + 1, secondComma - (secondEquals + 1))))};
            int yVel {std::stoi(std::string(line.substr(secondComma + 1)))};

            positions.push_back({xPos, yPos});
            velocities.push_back({xVel, yVel});
        }
        assert ((positions.size() == velocities.size()) && "There are not the same number of starting positions and velocities.\n");

        Robots robots {};
        for (size_t i{0}; i < positions.size(); ++i)
            robots.push_back({positions[i], velocities[i]});
        return robots;
    }

    int getNewPositionWithWrapAround(int position, int velocity, int size)
    {
        int newPos {position + velocity};
        int shift = (newPos < 0 ? size : -size);
        while (newPos < 0 || newPos >= size)
            newPos += shift;
        return newPos;
    }

    bool isRobotInPosition(const VectorPairs& positions, int x, int y)
    {
        for (std::pair<int, int> position : positions)
        {
            if (position.first == x && position.second == y)
                return true;
        }
        return false;
    }

    void printTree(const VectorPairs& positions, std::pair<int, int> map)
    {
        std::vector<std::string> tree {};
        for (int i{0}; i < map.second; ++i)
        {
            std::string row {};
            for (int j{0}; j < map.first; ++j)
            {
                row += '.';
            }
            tree.push_back(row);
        }
        for (std::pair<int, int> position : positions)
            tree[position.second][position.first] = '*';
        for (int y{0}; y < tree.size(); ++y)
        {
            for (int x{0}; x < tree[y].size(); ++x)
                std::cout << tree[y][x];
            std::cout << '\n';
        }
    }

    // this is a mess, but it gets the job done
    bool isTree(const VectorPairs& positions, std::pair<int, int> map)
    {
        for (std::pair<int, int> position : positions)
        {
            if (position.first > 3 && position.first < map.first - 4 && position.second > 3 && position.second < map.second - 4)
            {
                if (isRobotInPosition(positions, position.first - 1, position.second + 1) && isRobotInPosition(positions, position.first + 1, position.second + 1))
                {
                    if (isRobotInPosition(positions, position.first - 2, position.second + 2) && isRobotInPosition(positions, position.first + 2, position.second + 2))
                    {
                        if (isRobotInPosition(positions, position.first - 3, position.second + 3) && isRobotInPosition(positions, position.first + 3, position.second + 3))
                        {
                            if (isRobotInPosition(positions, position.first - 4, position.second + 4) && isRobotInPosition(positions, position.first + 4, position.second + 4))
                            {
                                printTree(positions, map);
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    int getTimeToTree(const Robots& robots, std::pair<int, int> map)
    {
        VectorPairs positions {};
        for (const Robot& robot : robots)
            positions.push_back(robot.first);

        int time {0};
        while (!isTree(positions, map))
        {
            ++time;
            for (int i{0}; i < robots.size(); ++i)
            {
                std::pair<int, int> velocity {robots[i].second};
                int x {getNewPositionWithWrapAround(positions[i].first, velocity.first, map.first)};
                int y {getNewPositionWithWrapAround(positions[i].second, velocity.second, map.second)};
                positions[i] = {x, y};
            }
        }
        return time;
    }

    template <std::size_t N>
    int parseAndGetTreeTime(const std::array<std::string_view, N>& lines, std::pair<int, int> map)
    {
        Robots robots {getStartingPositionsAndVelocities<N>(lines)};
        return getTimeToTree(robots, map);
    }
}

#endif // AOC14B_H

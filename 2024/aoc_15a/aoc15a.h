#ifndef AOC15A_H
#define AOC15A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>

#include <iostream>

/*
get room (warehouse)
get robot move instructions
find robot starting position
travel in each direction sequentially
don't move where hitting a wall
track movement of boxes when pushing them
identify where boxes (single or a chain) are against a wall and so not movement
get and return box locations at end
*/

// define in global space since used in different namespaces
using Direction = std::pair<int, int>;

namespace constants
{
    constexpr int yGPS {100};
}

namespace direction
{
    constexpr Direction up    { 0, -1};
    constexpr Direction right { 1,  0};
    constexpr Direction down  { 0,  1};
    constexpr Direction left  {-1,  0};
}

namespace aoc15a
{
    using Map = std::vector<std::vector<char>>;
    using Moves = std::vector<std::string_view>;
    using Position = std::pair<size_t, size_t>;

    Direction getDirection(char c)
    {
        switch (c)
        {
        case '^': return direction::up;
        case '>': return direction::right;
        case 'v': return direction::down;
        case '<': return direction::left;
        default : throw std::invalid_argument("Invalid direction.\n");
        }
    }

    template <std::size_t N>
    Map getWarehouseMap(const std::array<std::string_view, N>& lines)
    {
        Map map {};
        for (std::string_view line : lines)
        {
            if (line.find('#') == std::string_view::npos)
                break;
            std::vector<char> row {line.begin(), line.end()};
            map.push_back(row);
        }
        return map;
    }

    bool isArrowOnLine(std::string_view line)
    {
        return (line.find('^') != std::string_view::npos || line.find('>') != std::string_view::npos || line.find('v') != std::string_view::npos || line.find('<') != std::string_view::npos);
    }

    template <std::size_t N>
    Moves getMovementInstructions(const std::array<std::string_view, N> lines)
    {
        Moves instructions {};
        for (std::string_view line : lines)
        {
            if (!isArrowOnLine(line))
                continue;
            instructions.push_back(line);
        }
        return instructions;
    }

    Position getRobotStartPosition(Map& map)
    {
        for (size_t y{0}; y < map.size(); ++y)
        {
            for (size_t x{0}; x < map[y].size(); ++x)
            {
                if (map[y][x] == '@')
                {
                    map[y][x] = '.';
                    return {x, y};
                }
            }
        }
        throw std::invalid_argument("Did not find robot start position.\n");
    }

    void handleBoxQueue(Map& positions, Position& robot, Direction move)
    {
        std::vector<int> queue {};
        Position pos {robot.first + move.first, robot.second + move.second};
        int counter {1};
        while (positions[pos.second][pos.first] != '.' && positions[pos.second][pos.first] != '#')
        {
            queue.push_back(counter++);
            pos.first += move.first;
            pos.second += move.second;
            // if box queue is directly against a wall, don't move
            if (positions[pos.second][pos.first] == '#')
                return;
        }
        // at this point, there should be boxes going into empty space
        // go over each block and 'push' them one space over (shift + 1)
        for (int shift : queue)
            positions[robot.second + (shift + 1) * move.second][robot.first + (shift + 1) * move.first] = 'O';
        // position of the first box in the queue should now be empty
        positions[robot.second + move.second][robot.first + move.first] = '.';
        // finally, update position of robot
        robot = {robot.first + move.first, robot.second + move.second};
    }

    Map trackBoxMovements(const Map& map, const Moves& instructions)
    {
        Map positions {map};
        // function has side effect to change robot start to be blank (not a wall and not a box)
        Position robot {getRobotStartPosition(positions)};
        for (std::string_view row : instructions)
        {
            for (char c : row)
            {
                Direction move {getDirection(c)};
                // get position of potential new position (after move)
                Position potential {robot.first + move.first, robot.second + move.second};
                // check if there's nothing there, in which case move
                if (positions[potential.second][potential.first] == '.')
                    robot = potential;
                // check if there's a wall there, in which case don't move
                else if (positions[potential.second][potential.first] == '#')
                    continue;
                // check if there's a box there, if there is:
                else if (positions[potential.second][potential.first] == 'O')
                {
                    Position nextPotential {potential.first + move.first, potential.second + move.second};
                    // 1) check if space beyond box, in which case move
                    if (positions[nextPotential.second][nextPotential.first] == '.')
                    {
                        positions[nextPotential.second][nextPotential.first] = 'O';
                        positions[potential.second][potential.first] = '.';
                        robot = potential;
                    }
                    // 2) check if wall beyond box, in which case keep still
                    else if (positions[nextPotential.second][nextPotential.first] == '#')
                        continue;
                    // 3) check if chain of boxes, in which case it depends
                    // there shouldn't be any other options
                    else
                        handleBoxQueue(positions, robot, move);
                }
                else
                    throw std::invalid_argument("An invalid position occurred.\n");
            }
        }
        return positions;
    }

    int getGPSCoordinates(const Map& positions)
    {
        int gps {0};
        for (size_t y{0}; y < positions.size(); ++y)
        {
            for (size_t x{0}; x < positions[y].size(); ++x)
            {
                if (positions[y][x] == 'O')
                    gps += constants::yGPS * y + x;
            }
        }
        return gps;
    }

    template <std::size_t N>
    int parseAndGetBoxGPSes(const std::array<std::string_view, N>& lines)
    {
        Map map {getWarehouseMap<N>(lines)};
        Moves instructions {getMovementInstructions<N>(lines)};
        Map boxPositions {trackBoxMovements(map, instructions)};
        // to visualise the final position
        // for (auto row : boxPositions)
        // {
        //     for (char c : row)
        //         std::cout << c;
        //     std::cout << '\n';
        // }
        return getGPSCoordinates(boxPositions);
    }
}

#endif // AOC15A_H

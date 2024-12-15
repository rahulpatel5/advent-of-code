#ifndef AOC15B_H
#define AOC15B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <set>

#include <iostream>

/*
read room as for part 1
make map wider when setting up scaled up map


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

namespace aoc15b
{
    using Map = std::vector<std::vector<char>>;
    using Moves = std::vector<std::string_view>;
    using Position = std::pair<size_t, size_t>;
    using Boxes = std::vector<std::vector<Position>>;

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

    Map extendMap(const Map& map)
    {
        Map extended {};
        for (const std::vector<char>& row : map)
        {
            std::vector<char> line {};
            for (char c : row)
            {
                if (c == '#')
                {
                    line.push_back('#');
                    line.push_back('#');
                }
                else if (c == '.')
                {
                    line.push_back('.');
                    line.push_back('.');
                }
                else if (c == 'O')
                {
                    line.push_back('[');
                    line.push_back(']');
                }
                else if (c == '@')
                {
                    line.push_back('@');
                    line.push_back('.');
                }
            }
            extended.push_back(line);
        }
        return extended;
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

    void handleHorizontalQueue(Map& positions, Position& robot, Direction move)
    {
        std::vector<int> queue {};
        std::vector<char> boxes {};
        Position pos {robot.first + move.first, robot.second + move.second};
        int counter {1};
        while (positions[pos.second][pos.first] != '.' && positions[pos.second][pos.first] != '#')
        {
            queue.push_back(counter++);
            boxes.push_back(positions[pos.second][pos.first]);
            pos.first += move.first;
            pos.second += move.second;
            // if box queue is directly against a wall, don't move
            if (positions[pos.second][pos.first] == '#')
                return;
        }
        // at this point, there should be boxes going into empty space
        // go over each block and 'push' them one space over (shift + 1)
        assert((queue.size() == boxes.size()) && "The number of boxes in the queue and number of boxes are not the same.\n");
        for (size_t i{0}; i < queue.size(); ++i)
            positions[robot.second + (queue[i] + 1) * move.second][robot.first + (queue[i] + 1) * move.first] = boxes[i];
        // position of the first box in the queue should now be empty
        positions[robot.second + move.second][robot.first + move.first] = '.';
        // finally, update position of robot
        robot = {robot.first + move.first, robot.second + move.second};
    }

    void handleVerticalQueue(Map& positions, Position& robot, Direction move)
    {
        std::vector<int> queue {};
        Boxes moveBoxes {};
        std::set<Position> loopBoxes {};
        loopBoxes.insert(robot);
        Position pos {robot.first + move.first, robot.second + move.second};
        bool spaceInFront {false};
        while (!spaceInFront)
        {
            std::set<Position> newBoxes {};
            spaceInFront = true;
            for (std::pair<size_t, size_t> loopBox : loopBoxes)
            {
                char nextPos {positions[loopBox.second + move.second][loopBox.first + move.first]};
                // if any of boxes have wall in front, the move fails
                if (nextPos == '#')
                    return;
                
                // if there isn't a space in front of the box, then the queue continues
                if (nextPos != '.')
                    spaceInFront = false;
                // if box ahead, need to ahead, depending on which side
                if (nextPos == '[')
                {
                    newBoxes.insert({loopBox.first + move.first, loopBox.second + move.second});
                    newBoxes.insert({loopBox.first + move.first + 1, loopBox.second + move.second});
                }
                else if (nextPos == ']')
                {
                    newBoxes.insert({loopBox.first + move.first, loopBox.second + move.second});
                    newBoxes.insert({loopBox.first + move.first - 1, loopBox.second + move.second});
                }
            }
            loopBoxes = newBoxes;
            std::vector<Position> convertBoxes {newBoxes.begin(), newBoxes.end()};
            moveBoxes.push_back(convertBoxes);
        }
        // at this point, boxes should be going into empty space
        // handle in reverse order as want to move boxes and fill empty space
        for (int i{static_cast<int>(moveBoxes.size() - 1)}; i >= 0; --i)
        {
            for (int j{0}; j < moveBoxes[i].size(); ++j)
            {
                // simulate moving the box to the new position
                positions[moveBoxes[i][j].second + move.second][moveBoxes[i][j].first + move.first] = positions[moveBoxes[i][j].second][moveBoxes[i][j].first];
                // backfill the box with empty space
                // if there's another box that replaces it, it will get moved here later
                positions[moveBoxes[i][j].second][moveBoxes[i][j].first] = '.';
            }
        }
        // finally, update position of robot
        robot = {robot.first + move.first, robot.second + move.second};
    }

    void handleBoxQueue(Map& positions, Position& robot, Direction move)
    {
        // handle horizontal movement, as that's easier (like part 1)
        if (move == direction::left || move == direction::right)
            return handleHorizontalQueue(positions, robot, move);
        else if (move == direction::up || move == direction::down)
            return handleVerticalQueue(positions, robot, move);
        else
            throw std::invalid_argument("Invalid direction used.\n");
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
                // check if there's a box there
                else if (positions[potential.second][potential.first] == '[' || positions[potential.second][potential.first] == ']')
                {
                    // if there is, need function to handle scenarios
                    // more complex than part 1, so need more specificity
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
                if (positions[y][x] == '[')
                    gps += constants::yGPS * y + x;
            }
        }
        return gps;
    }

    template <std::size_t N>
    int parseAndGetBoxGPSes(const std::array<std::string_view, N>& lines)
    {
        Map map {getWarehouseMap<N>(lines)};
        Map extended {extendMap(map)};
        Moves instructions {getMovementInstructions<N>(lines)};
        Map boxPositions {trackBoxMovements(extended, instructions)};
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

#endif // AOC15B_H

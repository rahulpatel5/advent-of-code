#ifndef AOC14A_H
#define AOC14A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>

/*
extract starting position and velocities of each robot
simulate position of each robot after 100s
need to use wrap-around for the map
at 100s, separate map into quadrants (not counting middle row or column)
multiply number of robots in each of the quadrants
*/

namespace aoc14a
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

    template <int T>
    VectorPairs getPositionsAfterTime(const Robots& robots, std::pair<int, int> map)
    {
        VectorPairs positions {};

        for (const Robot& robot : robots)
        {
            std::pair<int, int> position {robot.first};
            std::pair<int, int> velocity {robot.second};
            for (int t{0}; t < T; ++t)
            {
                int x {getNewPositionWithWrapAround(position.first, velocity.first, map.first)};
                int y {getNewPositionWithWrapAround(position.second, velocity.second, map.second)};
                position = {x, y};
            }
            positions.push_back(position);
        }

        assert((positions.size() == robots.size()) && "Number of final positions doesn't match the original number of robots.\n");
        return positions;
    }

    int getSafetyFactor(const VectorPairs& positions, std::pair<int, int> map)
    {
        int middleRow {(map.second + 1) / 2 - 1};
        int middleCol {(map.first + 1) / 2 - 1};
        int quadrantTopLeft {0};
        int quadrantTopRight {0};
        int quadrantBotLeft {0};
        int quadrantBotRight {0};

        for (std::pair<int, int> position : positions)
        {
            if (position.first < middleCol && position.second < middleRow)
                ++quadrantTopLeft;
            else if (position.first > middleCol && position.second < middleRow)
                ++quadrantTopRight;
            else if (position.first < middleCol && position.second > middleRow)
                ++quadrantBotLeft;
            else if (position.first > middleCol && position.second > middleRow)
                ++quadrantBotRight;
        }
        return quadrantTopLeft * quadrantTopRight * quadrantBotLeft * quadrantBotRight;
    }

    template <std::size_t N, int T>
    int parseAndGetSafetyFactor(const std::array<std::string_view, N>& lines, std::pair<int, int> map)
    {
        Robots robots {getStartingPositionsAndVelocities<N>(lines)};
        VectorPairs finalPositions {getPositionsAfterTime<T>(robots, map)};
        return getSafetyFactor(finalPositions, map);
    }
}

#endif // AOC14A_H
